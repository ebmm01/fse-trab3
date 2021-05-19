export default {
    namespaced: true,
    state: {
        alarmEnabled: false
    },
    mutations: {
        setAlarm(state, alarm) {
            state.alarmEnabled = alarm
        }
    },
    actions: {
        setAlarm({ commit }, alarm) {
            commit('setAlarm', alarm)
        }
    }
}