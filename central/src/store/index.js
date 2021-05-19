import Vue from 'vue'
import Vuex from 'vuex'
import alarm from "./alarm"

Vue.use(Vuex)

export default new Vuex.Store({
    modules: {
        alarm
    }
})
