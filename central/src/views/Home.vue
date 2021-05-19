<template>
    <div>

        <v-row class="mt-5">
            <v-col
                cols="12"
                sm="12"
                md="12"
                lg="12">
                <h2 class="mb-5">
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
                                class="ml-3 no-action"
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
                            @click="removeDevice(item)" />
                    </template>
                </v-data-table>

            </v-col>
        </v-row>
        <hr class="my-5">
        <v-row>
            <v-col
                cols="12"
                sm="6"
                md="6"
                lg="6">
                <div class="d-flex align-center mb-5">
                    <h2>
                        Histórico
                    </h2>
                    <v-spacer></v-spacer>
                    <v-btn
                        v-if="actions.length"
                        @click="convertToCSV"
                        color="primary">
                        Baixar CSV
                    </v-btn>
                </div>
                <h3
                    v-if="!actions.length">
                    Não há ações registradas até o momento
                </h3>
                <v-data-table
                    v-else
                    :headers="headersActions"
                    :items="actions"
                    :items-per-page="10"
                    class="elevation-1" />
            </v-col>
            <v-col
                cols="12"
                sm="6"
                md="6"
                lg="6">
                <h2 class="mb-5">
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
                    :items-per-page="20"
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
        </v-row>
        <new-client-dialog
                @close="newClientDialog = false"
                @confirm="addDevice"
                max-width="500px"
                :device-id="currDeviceId"
                :active="newClientDialog" />
    </div>
</template>

<script>
import NewClientDialog from '../components/dialogs/NewClientDialog.vue'
import ButtonWithTooltip from '../components/utils/ButtonWithTooltip.vue'
import { mapState } from "vuex"
import csvHandler from "../utils/csv"
import beep from "../assets/beep.wav"

const topicName = `fse2020/${process.env.VUE_APP_MATRICULA}`
const alarm = new Audio(beep);


export default {
    name: 'Home',
    computed: {
        ...mapState('alarm', ['alarmEnabled'])
    },
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
            headersActions: [
                { text: 'Id dispositivo', value: 'mac' },
                { text: 'Cômodo', value: 'comodo' },
                { text: 'Ação', value: 'action' },
                { text: 'TimeStamp', value: 'timestamp' },
            ],
            actions: [],
            esps: [],
            headersSol: [
                {   text: 'Id', value: 'id' },
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
                console.log(topic)
                if (topic.split("/")[3] === 'estado') {
                    const value = +msg['entradaState']
                    elem['entradaValue'] = value

                    if (this.alarmEnabled) {
                        if (value)
                            alarm.play()
                        else 
                            alarm.pause()
                        
                        this.actions.push({
                            mac: elem.id,
                            comodo: elem.comodo,
                            action: elem.entradaValue ? 'ALARM_ON' : 'ALARM_OFF',
                            timestamp: (new Date()).toISOString()
                        })
                    }

                }
                else 
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

            if (this.esps.find(elem => elem.comodo === val.comodo)) {
                this.$notify({
                    type: 'warn',
                    text: 'Cômodo já adicionado'
                })
            }
            else {
                val.umidade = 0
                val.temperatura = 0
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
            }
        },
        updateSaidaValue(item) {
            this.$mqtt.publish(`${topicName}/dispositivos/${item.id}`, JSON.stringify({ 
                event: "UPDATE_OUTPUT_SENSOR",
                value: item.saidaValue
            }))

            this.actions.push({
                mac: item.id,
                comodo: item.comodo,
                action: item.saidaValue? 'ON' : 'OFF',
                timestamp: (new Date()).toISOString()
            })
        },
        formatOutput(value, type) {
            if (+value <= 0 || isNaN(value))
                return "NaN"
            if (type === 'temp') 
                return `${(+value/10)} ºC`
            else
                return `${(+value/10)}%`
        },
        convertToCSV() {
            csvHandler('historico.csv', this.actions)
        },
        removeDevice(item) {
            if (confirm("Tem certeza que deseja remover esse dispotivo?")) {
                const id = item.id 

                this.esps.splice(this.esps.findIndex(elem => elem.id === id), 1)

                this.$mqtt.publish(`${topicName}/dispositivos/${id}`, JSON.stringify({ 
                    event: "RESET_DEVICE"
                }))
            }
        }
    },
    created() {
        this.$mqtt.subscribe(`${topicName}/dispositivos/#`)
        this.$mqtt.subscribe(`${topicName}/+/#`)
    }
}
</script>

<style>
.no-action {
    pointer-events: none !important;
}
</style>