import Vue from 'vue'
import App from './App.vue'
import router from './router'
import "element-ui/lib/theme-chalk/index.css";
import ElementUI from "element-ui";
import { Loading } from 'element-ui'
import * as echarts from "echarts";
import axios from 'axios'
import 'lib-flexible/flexible'

// request interceptor
axios.interceptors.request.use(

    config => {
      //Determine the type of request
      // if it is a post request, the default parameters are spelled into data
      // if it is a get request, they are spelled into params
      if(config.method==='post'){
        const url = sessionStorage.getItem("url")
        config.data=JSON.stringify({
          url,
          ...config.data
        })
      }else if(config.method==='get'){
        const url = sessionStorage.getItem("url")
        config.params={
          url,
          ...config.params
        }
      }
      return config;
    }
);


Vue.prototype.$axios = axios    //Global Registration

Vue.prototype.$echarts = echarts;

Vue.config.productionTip = false;

// Get cached data globally
Vue.prototype.resetSetItem = function (key: string, newVal: any) {
  if (key === 'url') {

    // Create a StorageEvent event
    var newStorageEvent = document.createEvent('StorageEvent');
		
    const storage = {
      setItem: function (k: string, val: string) {
        sessionStorage.setItem(k, val);
				
				newStorageEvent.initEvent('setItem', false, false)
        // Initialize the created event
        // newStorageEvent.initStorageEvent('setItem', false, false, k, null, val, null, null);

        // Distribution object
        window.dispatchEvent(newStorageEvent)
      }
    }
    return storage.setItem(key, newVal);
  }
}

Vue.prototype.openLoading = function() {
  const loading = this.$loading({        
    lock: true,                          
    text: 'LOADING...',                  
    spinner: 'el-icon-loading',          
    background: 'rgba(0, 0, 0, 0.3)',    
    target: '.sub-main',                 
    body: true,                              
    customClass: 'mask'                  
  })
  setTimeout(function () {               
    loading.close();                     
  },10000)
  return loading;
}


Vue.use(ElementUI, {
  size: "mini",
});

new Vue({
  router,
  render: h => h(App)
}).$mount('#app')