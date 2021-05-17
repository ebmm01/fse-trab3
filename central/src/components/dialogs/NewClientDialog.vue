<template>
    <card-dialog
        v-bind="{...$props, ...$attrs}"
        :active="active">
        <template v-slot:header>
            Novo client
        </template>

        <v-form
            ref="form"
            v-model="valid"
            lazy-validation>
            <v-text-field
                v-model="comodo"
                :rules="[v => !!v || 'O nome não pode estar em branco']"
                label="Cômodo"
                required />
            <v-text-field
                v-model="entrada"
                :rules="[v => !!v || 'O nome não pode estar em branco']"
                label="Nome dispositivo entrada"
                required />
            <v-text-field
                v-model="saida"
                :rules="[v => !!v || 'O nome não pode estar em branco']"
                label="Nome dispositivo saida"
                required />
            <v-select
                v-model="type"
                :items="items"
                item-text="text"
                item-value="value"
                label="Tipo" />
        </v-form>
        <template v-slot:footer>
            <v-btn
                color="secondary"
                text
                @click="closeDialog">
                Cancelar
            </v-btn>
            <v-spacer />
            <v-btn
                color="warning"
                text
                @click="confirmSubmit">
                Confirmar
            </v-btn>
        </template>
    </card-dialog>    
</template>

<script>
import CardDialog from "./CardDialog.vue";

export default {
    props: {
        active: {
            type: Boolean,
            required: true
        },
        deviceId: {
            type: String,
            required: false,
            default: ''
        }
    },
    data() {
        return {
            valid: true,
            comodo: "",
            entrada: "",
            saida: "",
            type: "",
            items: [
                { text: 'Energia', value: 'energia' },
                { text: 'Bateria', value: 'bateria' },
            ]
        }
    },
    components: {
        CardDialog
    },
    methods: {
        closeDialog() {
            this.$emit('close')
        },
        confirmSubmit() {
            if (this.$refs.form.validate())
                this.$emit('confirm', { 
                    comodo: this.comodo, 
                    entrada: this.entrada,
                    saida: this.saida,
                    id: this.deviceId,
                    type: this.type,
                    status: 1
                })
        }
    }
}
</script>