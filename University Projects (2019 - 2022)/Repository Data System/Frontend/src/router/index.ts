import Vue from 'vue'
import Router from 'vue-router'
import Group from '../views/group/group.vue'
import Personal from '../views/personal/personal.vue'
import Index from '../views/index.vue'
import Container from '../views/container.vue'

Vue.use(Router)

export default new Router({
    routes: [
        {
            path: '/',

            component: Container,
            children: [
                {path: '', name: 'Summary', component: Index,},
                {path: 'group', name: 'Group', component: Group,},
                {path: 'personal', name: 'Personal', component: Personal,},
            ]
        }
    ]
})
