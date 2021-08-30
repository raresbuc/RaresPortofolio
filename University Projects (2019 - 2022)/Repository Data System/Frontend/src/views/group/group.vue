<template>
  <el-scrollbar>
    <div class="view-container">
      <el-row style="margin: 2%">
        <el-col>
          <div ref="chart" style="width: 100%; height: 450px"></div>
        </el-col>
      </el-row>
      <el-row style="margin: 2%">
        <el-col :span="12">
          <div ref="chart2" style="width: 100%; height: 350px; margin-top: 5%"></div>
        </el-col>
        <el-col :span="12">
          <div ref="chart3" style="width: 100%; height: 320px; margin-top: 5%"></div>
        </el-col>
      </el-row>
    </div>
  </el-scrollbar>
</template>

<script>
export default {
  data() {
    return {
      value: "Project Name",
      projectData: [],
      repoLang: [],
      week: [],
      issueData: [],
      rLoading: '',
    };
  },
  created() {

      window.addEventListener('setItem', ()=> {
        this.rLoading = this.openLoading()
        this.getRepoLang();
        this.getProjectData();

        // [NEW]: add get issues method.
        this.getIssues()
      })

    this.rLoading = this.openLoading()
    //Get all project information
    this.getRepoLang();

    this.getProjectData();

    // test for get all issues api.
    this.getIssues()
  },
  mounted() {

  },
  methods: {
    // get issues data finally and then render the chart of all items on the page.
    getIssues() {
      this.$axios
        .get("api/getIssues")
        .then((response) => {
          this.issueData = response.data.data[0]
          
          // init the echart with data.
          this.initChart();

          this.rLoading.close();
        })
    },
    getRepoLang() {
      this.$axios
        .get("api/getRepoLang")
        .then((response) => {
         
          this.repoLang = response.data.data
          // console.log(response.data.data);
        })
        .catch((response) => {
          
        });
    },
    //Cross-domain access with cm-api as a proxy
    getProjectData() {
      this.$axios
        .get("api/getAllProjects")
        .then((response) => {
          this.week = [];
          //Get the interface data and assign it to the data object
          this.projectData = response.data.data;
          console.log('project data::::::')
          console.log(this.projectData)
          for (let index = 0; index < response.data.data.length; index++) {
            this.week.push("week"+(index+1))
            
          }
          // console.log(this.week)
          // this.initChart();

          // console.log("data");
        })
        .catch((response) => {
          // console.log(response);
        });
    },

    initChart() {
      let lineChart = this.$echarts.init(this.$refs.chart);
      this.option = lineChart.setOption({
        title: {
          text: "LINES OF CODE CHANGED PER WEEK",
        },

        grid: {
          left: "3%",
          right: "4%",
          bottom: "3%",
          containLabel: true,
        },
        toolbox: {
          show: true,
          right: 50,

          feature: {
            saveAsImage: { title: "save as image" },
            dataView: { readOnly: false, title: "data view" },
          },
        },
        xAxis: {
          type: "category",
          boundaryGap: false,
          data: this.week,
        },
        yAxis: {
          type: "value",
        },
        series: [
          {
            type: "line",
            data: this.projectData,
          },
        ],
      });

      let lineChart2 = this.$echarts.init(this.$refs.chart2);
      lineChart2.setOption({
        title: {
          // text: "Issues(TOTAL:" + this.issueData.All + ')',
          text: "ISSUES（PULL REQUEST INCLUDED）",
        },
        tooltip: {
          trigger: "axis",
        },
        toolbox: {
          show: true,
          right: 50,
          feature: {
            dataView: { show: true, readOnly: false, title: "data view" },
            saveAsImage: { show: true, title: "save as image" },
          },
        },
        calculable: true,
        xAxis: [
          {
            type: "category",
            axisLabel: {
              interval: 0,
            },
            data: [
              "All Issues",
              "Open Issues",
              "Closed Issues"],
          },
        ],
        yAxis: [
          {
            type: "value",
          },
        ],
        series: [
          {
            name: "Lines",
            type: "bar",
            data: [this.issueData.All, this.issueData.Open, this.issueData.Closed],
            markPoint: {
              data: [
                { type: "max", name: "max value" },
                { type: "min", name: "min value" },
                { type: "average", name: "average value" },
              ],
            },
            // markLine: {
            //   data: [{ type: "average", name: "av value" }],
            // },
          },
        ],
      });

      let lineChart3 = this.$echarts.init(this.$refs.chart3);
      lineChart3.setOption({
        title: {
          text: "LANGUAGES",
          
          left: "center",
        },
        tooltip: {
          trigger: "item",
        },
        legend: {
          orient: "vertical",
          left: "left",
        },
        series: [
          {
            name: "Source",
            type: "pie",
            radius: "50%",
            data: this.repoLang,
            emphasis: {
              itemStyle: {
                shadowBlur: 10,
                shadowOffsetX: 0,
                shadowColor: "rgba(0, 0, 0, 0.5)",
              },
            },
          },
        ],
      });
    },
  },
};
</script>
<style></style>
