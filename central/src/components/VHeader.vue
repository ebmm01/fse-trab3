<template>
    <v-app-bar
      app
      color="primary"
      dark
      class="px-12"
    >
        <div class="d-flex align-center">
            <v-icon>mdi-gear</v-icon>
            <h2 class="title">Dashboard</h2>
        </div>

        <v-spacer></v-spacer>
        <v-menu
            offset-y
            bottom
            :close-on-content-click="false">
            <template v-slot:activator="{ on: menu, attrs }">
                <v-tooltip bottom>
                <template v-slot:activator="{ on: tooltip }">
                    <v-btn
                        color="white"
                        icon
                        v-bind="attrs"
                        v-on="{ ...tooltip, ...menu }"
                        >
                        <v-icon>mdi-cog</v-icon>
                    </v-btn>
                </template>
                <span>Configurações</span>
                </v-tooltip>
            </template>
            <v-list
                width="200">
                <v-list-item>
                    <v-list-item-title>
                        <div class="d-flex flex-column">
                            <p class="caption mb-0">Alarme sonoro</p>
                            <v-switch
                                class="ml-3"
                                v-model="alarm"
                                inset
                                @change="updateAlarm"
                                :label="alarmEnabled ? 'Habilitado' : 'Desabilitado'"/>
                        </div>
                    </v-list-item-title>
                </v-list-item>
            </v-list>
        </v-menu>
    </v-app-bar>
</template>

<script>
import { mapState, mapActions} from "vuex"

export default {
    data() {
        return {
            alarm: false
        }
    },
    computed: {
        ...mapState('alarm', ['alarmEnabled'])
    },
    methods: {
        ...mapActions('alarm', ['setAlarm']),
        updateAlarm(val) {
            this.setAlarm(val)
        }
    }
}
</script>