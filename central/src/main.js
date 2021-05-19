import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import vuetify from './plugins/vuetify'
import VueMqtt from 'vue-mqtt';
import Notifications from 'vue-notification'


Vue.use(VueMqtt, process.env.VUE_APP_MQTT_HOST);
Vue.use(Notifications)

Vue.config.productionTip = false

new Vue({
  router,
  store,
  vuetify,
  render: h => h(App)
}).$mount('#app')
