<template>
    <v-dialog
        v-bind="{...$props, ...$attrs}"
        v-model="active"
        persistent>
        <v-card>
            <v-card-title 
                class="headline card-title">
                <slot name="header" />
            </v-card-title>

            <v-card-text>
                <slot />    
            </v-card-text>

            <v-card-actions>
                <slot name="footer">
                    <v-spacer />
                    <v-btn
                        color="danger"
                        text
                        @click="closeDialog">
                        Cancelar
                    </v-btn>
                </slot>
            </v-card-actions>
        </v-card>
    </v-dialog>
</template>

<script>
export default {
    props: {
        active: {
            required: true,
            type: Boolean
        }
    },
    methods: {
        closeDialog() {
            this.$emit('close')
        }
    }
}
</script>

<style lang="scss">
.v-card__title {
    &.card-title {
        background-color: #f9fafd !important;
    }
}
</style>