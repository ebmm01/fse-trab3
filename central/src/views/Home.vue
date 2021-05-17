<template>
    <v-row>
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

export default {
    name: 'Home',
    data() {
        return {
            newClientDialog: false,
            headers: [
                {
                    text: 'Id',
                    value: 'id'
                },
                { text: 'Cômodo', value: 'comodo' },
                { text: 'Status', value: 'status' },
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
        'fse2020/150009011/dispositivos/+': function(val) {
            const msg = JSON.parse(new TextDecoder().decode(val))

            const elem = this.esps.find(elem => elem.id === msg.id)
            if (!elem)
                this.espsSol.push({id: msg.id})
            else  {
                const updatedElem = {...elem}
                updatedElem.status = 1
                this.esps.splice(this.esps.findIndex(elem => elem.id === msg.id), 1, updatedElem)
                console.log('Updated!!')
            }
        },
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
            this.esps.push(val)
            this.currDeviceId = ""
            this.newClientDialog = false
            this.espsSol.splice(this.espsSol.findIndex(elem => elem.id === val.id), 1)
        }
    },
    created() {
        this.$mqtt.subscribe('fse2020/150009011/dispositivos/param')
    }
}
</script>
