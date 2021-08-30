module.exports = {
    devServer: {
        proxy: {
            // Configuring cross-domains
            '/cm-api': {
                target: 'http://ec2-34-201-3-10.compute-1.amazonaws.com:8080/',
                ws: true,
                changOrigin: true,
                pathRewrite: {
                    '^/cm-api': ''
                }
            },
            // Configuring cross-domains
            '/api': {
                target: 'http://speacserver-env.eba-qvk95bfq.us-east-1.elasticbeanstalk.com',
                ws: true,
                changOrigin: true,
                pathRewrite: {
                    '^/api': ''
                }
            }
        },

    },
    chainWebpack: config => {
        config.module
            .rule('css')
            .test(/\.css$/)
            .oneOf('vue')
            .resourceQuery(/\?vue/)
            .use('px2rem')
            .loader('px2rem-loader')
            .options({
                remUnit: 75
            })
    },
}
