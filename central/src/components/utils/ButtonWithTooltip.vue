<template>
    <v-tooltip
        v-bind="{...$props, ...$attrs}">
        <template v-slot:activator="{ on }">
            <v-btn
                v-bind="{...$props, ...$attrs}"
                v-on="on"
                icon
                :color="$attrs['btn-color']"
                depressed
                @click="emitClicked">
                <v-icon
                    :color="$attrs['icon-color'] || 'primary'"
                    :small="$attrs['small'] || false"
                    :large="$attrs['large'] || false">
                    {{ icon }}
                </v-icon>
            </v-btn>
        </template>
        <span>{{ label }}</span>
    </v-tooltip>
</template>

<script>
export default {
    props: {
        label: {
            required: true,
            type: String
        },
        icon: {
            required: true,
            type: String
        }
    },
    methods: {
        emitClicked() {
            this.$emit('click')
        }
    }
}
</script>