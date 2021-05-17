<template>
    <v-row class="mt-5">
        <v-col
            cols="12"
            sm="9"
            md="9"
            lg="9">
            <h2>
                Clients
            </h2>
            <h3
                v-if="!esps.length">
                Não há dispositivos cadastrados
            </h3>
            <v-data-table
                v-else
                :headers="headers"
                :items="esps"
                :items-per-page="esps.length"
                class="elevation-1">
                <template v-slot:item.entrada="{ item }">
                    <div class="d-flex align-center">
                        {{ item.entrada }}
                        <v-switch
                            class="ml-3"
                            v-model="item.entradaValue"
                            inset
                            readonly
                            :label="item.entradaValue ? 'on' : 'off'"/>
                    </div>
                </template>
                <template v-slot:item.saida="{ item }">
                    <div class="d-flex align-center">
                        {{ item.saida }}
                        <v-switch
                            class="ml-3"
                            v-model="item.saidaValue"
                            inset
                            @change="updateSaidaValue(item)"
                            :label="item.saidaValue ? 'on' : 'off'"/>
                    </div>
                </template>
                <template v-slot:item.temperatura="{ item }">
                    <div class="d-flex align-center">
                        {{ formatOutput(item.temperatura, 'temp') }}
                    </div>
                </template>
                <template v-slot:item.umidade="{ item }">
                    <div class="d-flex align-center">
                        {{ formatOutput(item.umidade, 'umidade') }}
                    </div>
                </template>
                <template v-slot:item.actions="{ item }">
                    <button-with-tooltip
                        large 
                        bottom
                        label="Remover"
                        icon="mdi-delete"
                        @click="newClientDialog=true" />
                </template>
            </v-data-table>

        </v-col>
        <v-col
            cols="12"
            sm="3"
            md="3"
            lg="3">
            <h2>
                Solicitações {{ espsSol.length || '' }}
            </h2>
            <h3
                v-if="!espsSol.length">
                Não há solicitações de clients pendentes
            </h3>
            <v-data-table
                v-else
                :headers="headersSol"
                :items="espsSol"
                :items-per-page="espsSol.length"
                hide-default-footer
                class="elevation-1">
                <template v-slot:item.actions="{ item }">
                    <button-with-tooltip
                        large 
                        bottom
                        label="Adicionar client"
                        icon="mdi-plus"
                        @click="addNewClient(item)" />
                </template>
            </v-data-table>
        </v-col>

        <new-client-dialog
            @close="newClientDialog = false"
            @confirm="addDevice"
            max-width="500px"
            :device-id="currDeviceId"
            :active="newClientDialog" />
    </v-row>
</template>

<script>
import NewClientDialog from '../components/dialogs/NewClientDialog.vue'
import ButtonWithTooltip from '../components/utils/ButtonWithTooltip.vue'

const topicName = `fse2020/${process.env.VUE_APP_MATRICULA}`

export default {
    name: 'Home',
    data() {
        return {
            newClientDialog: false,
            headers: [
                { text: 'Id', value: 'id' },
                { text: 'Cômodo', value: 'comodo' },
                { text: 'Tipo', value: 'type' },
                { text: 'Entrada', value: 'entrada' },
                { text: 'Saida', value: 'saida' },
                { text: 'Temperatura', value: 'temperatura' },
                { text: 'Umidade', value: 'umidade' },
                { text: 'Ações', value: 'actions' }
            ],
            esps: [ ],
            headersSol: [
                {
                    text: 'Id',
                    value: 'id'
                },
                { text: 'Ações', value: 'actions' }
            ],
            espsSol: [],
            currDeviceId: ''
        }
    },
    mqtt: {
        [`${topicName}/dispositivos/#`]: function(val, topic) {
            const msg = JSON.parse(new TextDecoder().decode(val))

            if (msg.id && msg.event !== "ADDED_DEVICE") {
                const elem = this.esps.find(elem => elem.id === msg.id) || this.espsSol.find(elem => elem.id === msg.id)
                if (!elem)
                    this.espsSol.push({id: msg.id})
                else  {
                    const updatedElem = {...elem}

                    updatedElem.status = 1
                    updatedElem.saidaValue = 0
                    updatedElem.entradaValue = 0

                    this.esps.splice(this.esps.findIndex(elem => elem.id === msg.id), 1, updatedElem)
                       
                    setTimeout(() => {
                        this.$mqtt.publish(topic, JSON.stringify({ 
                            event: "ADDED_DEVICE",
                            room: updatedElem.comodo
                        }))
                    }, 2000)
                }
            }
        },
        [`${topicName}/+/#`] (val, topic) {
            const topicName = topic.split("/")[2]
            const isDevice = topicName === 'dispositivos'
            const msg = JSON.parse(new TextDecoder().decode(val))
            
            const elem = this.esps.find(elem => elem.comodo === topicName)

            if (!isDevice && elem) {

                elem[topic.split("/")[3]] = msg[topic.split("/")[3]]

                this.esps.splice(this.esps.findIndex(elem => elem.comodo === topicName), 1, elem)
            }
        }
    },
    components: {
        NewClientDialog,
        ButtonWithTooltip
    },
    methods: {
        addNewClient(client) {
            this.currDeviceId = client.id
            this.newClientDialog = true
        },
        addDevice(val) {
            val.saidaValue = 0
            val.entradaValue = 0

            this.esps.push(val)
            
            this.currDeviceId = ""
            this.newClientDialog = false

            this.espsSol = this.espsSol.filter(elem => elem.id !== val.id)
        
        
            this.$mqtt.publish(`${topicName}/dispositivos/${val.id}`, JSON.stringify({ 
                event: "ADDED_DEVICE",
                room: val.comodo
            }))
        },
        updateSaidaValue(item) {
            this.$mqtt.publish(`${topicName}/dispositivos/${item.id}`, JSON.stringify({ 
                event: "UPDATE_OUTPUT_SENSOR",
                value: item.saidaValue
            }))
        },
        formatOutput(value, type) {
            if (type === 'temp') 
                return `${(+value/10)} ºC`
            else
                return `${(+value/10)}%`
        }
    },
    created() {
        console.log(topicName)
        this.$mqtt.subscribe(`${topicName}/dispositivos/#`)
        this.$mqtt.subscribe(`${topicName}/+/#`)
    }
}
</script>
