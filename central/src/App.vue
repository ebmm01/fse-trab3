<template>
  <v-app>
    <v-header></v-header>

    <v-main>
      <v-container>
        <router-view/>

      </v-container>
    </v-main>
    <notifications position="top center"/>
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
