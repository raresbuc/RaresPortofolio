<template>
  <div>
    
    <header class="section_title">{{ description }}</header>
    <section style=" padding: 20px; margin-bottom: 40px"
    >
      <el-row :gutter="20" style="margin-bottom: 10px;">
        <el-col :span="4">
          <div class="data_list today_head"><span class="data_num head">Today</span></div>
        </el-col>
        <el-col :span="4">
          <div class="data_list">{{ commits }} Commits</div>
        </el-col>
        <el-col :span="4">
          <div class="data_list">{{ issues }} Issues</div>
        </el-col>
      </el-row>
      <el-row :gutter="20" style="margin-bottom: 10px;">
        <el-col :span="4">
          <div class="data_list all_head"><span class="data_num head">Total</span></div>
        </el-col>
        <el-col :span="4">
          <div class="data_list">{{ aCommits }} Commits</div>
        </el-col>
        <el-col :span="4">
          <div class="data_list">{{ aIssues }} Issues</div>
        </el-col>
        <el-col :span="4">
          <div class="data_list">{{ branches }} Branches</div>
        </el-col>
        <el-col :span="4">
          <div class="data_list">{{ contributors }} Contributors</div>
        </el-col>
      </el-row>
    </section>
    <el-table
        :data="tableData"
        style="width: 100%">
      <el-table-column
          label="Commit"
          prop="commit"
          width="650"
      >
      </el-table-column>
      <el-table-column
          label="User"
          prop="user"
          width="180">
      </el-table-column>
      <el-table-column
          label="Time"
          prop="time"
      >
      </el-table-column>
    </el-table>
  </div>
</template>

<script>
export default {
  data() {
    return {
      description: "",
      commits: 2, aCommits: 11, issues: 3, aIssues: 5, branches: 3, contributors: 6,
      tableData: [{
        commit: 'test',
        user: 'user',
        time: '01/02/2021 15:59'
      }, {
        commit: 'test',
        user: 'user',
        time: '01/02/2021 15:59'
      }, {
        commit: 'test',
        user: 'user',
        time: '01/02/2021 15:59'
      }, {
        commit: 'test',
        user: 'user',
        time: '01/02/2021 15:59'
      }, {
        commit: 'test',
        user: 'user',
        time: '01/02/2021 15:59'
      }, {
        commit: 'test',
        user: 'user',
        time: '01/02/2021 15:59'
      },]
    }
  },
  methods: {
    created() {
    //Get all project information
  },
    
  },
}
</script>

<style>

.section_title {
  text-align: left;
  font-size: 30px;
  margin: 25px;
}

.data_list {
  font-size: 30px;
  padding: 20px 0;
  color: #666;
  border-radius: 15px;
  background: #E5E9F2;
}

.today_head {
  background: #FF9800;
  color: #fff;
}

.all_head {
  background: #20A0FF;
  color: #fff;
}
</style>
