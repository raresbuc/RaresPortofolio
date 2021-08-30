<template>
  <div>
    <el-container>
      <el-aside width="10%">
        <img height="70"
             src="@/assets/logo.png"
             width="70"
        />
        <el-menu
            router
        >
          <template
            v-for="route in $router.options.routes"
            v-if="route.children && route.children.length"
          >
            <template v-for="item in route.children">
              <el-menu-item
                  :index="route.path + item.path"
              >
                <span slot="title">{{ item.name }}</span>
              </el-menu-item>
            </template>
          </template>
        </el-menu>
      </el-aside>

      <el-container>
        <el-header>
          <el-row style="margin: 1%">
            <el-col>
              <span style="font-weight: bold; margin-right: 10px; font-size:20px">{{
                  defaultValue
              }}</span>

              <el-select
                v-model="defaultValue"
                placeholder="select project"
                @change="change"
                :popper-append-to-body="false"
              >
                <el-option
                  v-for="item in projectData"
                 :key="item.id"
                    :label="item.projectName"
                    :value="item.projectName"
                />
              </el-select>
            </el-col>
          </el-row>
        </el-header>
        <el-main class="app-body">
          <template>
            <router-view />
          </template>
        </el-main>
      </el-container>
    </el-container>
  </div>
</template>
<script>
export default {
  data() {
    return {
      projectData: [],
      defaultValue: ""

    };
  },
  created() {
    //Get all project information
    this.getProjectData();
    //sessionStorage.setItem('url',response.data[0].projectName); // Deposit a value
  },

  mounted() {


  },
  //Cross-domain access with cm-api as a proxy
  methods: {
    change(value) {
      //renew value
      this.resetSetItem('url',value); // Deposit a value
      //location.reload()
      setTimeout(()=>{   //set delay execution
        window.addEventListener('setItem', ()=> {
          this.defaultValue = sessionStorage.getItem('url');
        })
      },1000);

      console.log( this.defaultValue);
    },
    getProjectData() {
      this.$axios
        .get("cm-api/getAllProjects")
        .then((response) => {
          //Get the interface data and assign it to the data object
          //window.sessionStorage.setItem('url',response.data[0].projectName); // Deposit a value
          this.resetSetItem('url',response.data[0].projectName);
          this.projectData = response.data;
          this.defaultValue = sessionStorage.getItem('url');


        })
        .catch((response) => {
          console.log(response);
        });
    },
  },
};
</script>
<style lang="less">
.el-input__inner{
    font-size: 20px !important;
  }
</style>
