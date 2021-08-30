<script src="src/main.ts"></script>
<template>
  <div>
    <el-row style="margin: 2%">
      <el-col>
        <div ref="chart" style="width: 100%; height: 450px"></div>
      </el-col>
    </el-row>
    <el-row>
      <div ref="chart2" style="width: 100%; height: 500px"></div>  
    </el-row>
    <el-row>
      <div ref="chart3" style="width: 100%; height: 500px; margin-top: 100px;"></div>  
    </el-row>
  </div>
</template>

<script>
export default {
  data() {
    return {
      totalCommit: [],
      userName: [],
      data:[],
      week: [],
      usrInfo:[],
      issueData: [],
      rLoading: '',
      };
  },
  created() {

    window.addEventListener('setItem', ()=> {
      // const rLoading = this.openLoading();
      this.rLoading = this.openLoading()
      this.getUserData();
      this.getIssues()
    })


    this.rLoading = this.openLoading()
    this.getUserData();
    this.getIssues()

  },
  mounted() {
    //Loading echarts

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
    getUserData() {
      this.$axios
        .get("api/getAllUser")
        .then((response) => {
          this.totalCommit=[];
          this.userName=[];
          this.usrInfo = [];
          this.week=[];
          this.data = response.data.data
          //Get the interface data and assign it to the data object
          
          for (let index = 0; index < response.data.data.length; index++) {
            this.usrInfo.push(response.data.data[index].usrInfo)
            this.userName.push(response.data.data[index].userName);
            this.totalCommit.push(response.data.data[index].totalCommit);
          }
          for (let index = 0; index < this.data[4].usrInfo.length; index++) {
            this.week.push("week"+(index+1))
          }
          console.log(this.usrInfo)
          // this.initChart();
        })
        .catch((response) => {
          console.log(response);
        });
    },

    // get all the values from assigness's issue object
    getObjectValues(o)
    {
        var values = [];
        for (var property in o) values.push(o[property]);
        return values;
    },

    handleAssigneeDataFormat(assignee) {
      let json_str = assignee.replaceAll('[', '').replaceAll(']', '')
      return json_str.split(',');
    },

    initChart() {

      let lineChart = this.$echarts.init(this.$refs.chart);
      lineChart.setOption({
        toolbox: {
          show: true,
          right: 50,

          feature: {
            saveAsImage: { title: "save as image" },
            dataView: { readOnly: false, title: "data view" },
          },
        },
        title: {
          text: "COMMITS",
        },
        xAxis: {
          type: "category",
          data: this.userName,
        },
        yAxis: {
          type: "value",
        },
        series: [
          {
            type: "bar",
            data: this.totalCommit,
            itemStyle: {
              normal: {
                label: {
                  show: true,
                  position: 'top',
                  textStyle: {
                    color: 'black',
                    fontSize: 16
                  }
                }
              }
            },
            showBackground: true,
            backgroundStyle: {
              color: "rgba(180, 180, 180, 0.2)",
            },
          },
        ],
      });
      let myChart = this.$echarts.init(this.$refs.chart2);
      myChart.setOption({
        title: {
          // text: "Lines Of Code Changed Per Week Per User",
          text: "LINES OF CODE CHANGED\nPER WEEK OF USER",
        },
        tooltip: {
          trigger: "axis",
        },
        legend: {
          data: this.userName,
        },
        grid: {
          left: "3%",
          right: "4%",
          bottom: "3%",
          containLabel: true,
        },
        toolbox: {
          feature: {
            saveAsImage: {},
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
            name: this.userName[0],
            type: "line",
            stack: "gross0",
            data: this.usrInfo[0],
          },
          {
            name: this.userName[1],
            type: "line",
            stack: "gross1",
            data: this.usrInfo[1],
          },
          {
            name: this.userName[2],
            type: "line",
            stack: "gross2",
            data: this.usrInfo[2],
          },
          {
            name: this.userName[3],
            type: "line",
            stack: "gross3",
            data: this.usrInfo[3],
          },
          {
            name: this.userName[4],
            type: "line",
            stack: "gross4",
            data: this.usrInfo[4],
          },
          {
            name: this.userName[5],
            type: "line",
            stack: "gross5",
            data:this.usrInfo[5],
          },
        ],
      });

      // issue data charts(horizontal direction with memebers in each issue)
      let issueChart = this.$echarts.init(this.$refs.chart3);
      issueChart.setOption({
        title: {
            text: 'PROJECT ISSUES(PULL REQUEST INCLUDED)',
            subtext: `TOTAL: ${this.issueData.All} / UNASSIGNED: ${this.issueData.Unassigned}`
        },
        tooltip: {
            trigger: 'axis',
            axisPointer: {
                type: 'shadow'
            }
        },
        legend: {
            data: ['OPEN-ISSUES', 'CLOSED-ISSUES']
        },
        grid: {
            left: '3%',
            right: '4%',
            bottom: '3%',
            containLabel: true
        },
        xAxis: {
            type: 'value',
            boundaryGap: [0, 0.01]
        },
        yAxis: {
            type: 'category',
            data: this.handleAssigneeDataFormat(this.issueData.AssigneeName)
        },
        series: [
            {
                name: 'OPEN-ISSUES',
                type: 'bar',
                data: this.getObjectValues(this.issueData.OpenIssues)
            },
            {
                name: 'CLOSED-ISSUES',
                type: 'bar',
                data: this.getObjectValues(this.issueData.ClosedIssues)
            }
        ]
      })
    }
  },
};


</script>

<style lang="less">
</style>

