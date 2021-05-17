<template>
  <v-app>
    <v-header></v-header>

    <v-main>
      <v-container>
        <router-view/>

      </v-container>
    </v-main>
  </v-app>
</template>

<script>
import VHeader from "./components/VHeader"


export default {
  name: 'App',
  components: {
    'v-header': VHeader
  },
  data: () => ({
    //
  }),
};
</script>
