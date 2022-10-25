<script setup lang="ts">
import TheWelcome from "../components/TheWelcome.vue";
import {ClientInstance} from "../client/MonitorClient";
setInterval(refresh, 100);

function refresh() {
  console.log("refresh " + ClientInstance.monitorValueQueue)
}
</script>

<template>
  <main>
    <!-- <TheWelcome /> -->
  </main>
</template>
