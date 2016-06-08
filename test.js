var thulac = require('./index.js');

segmentor = new thulac.segmentor.Segmentor();

console.log('loading model ...');
segmentor.loadModel({
    t2s: false,
    seg_only: false,
    filter: true,
    delimeter: '#',
    model_dir: './models/'
});

console.log('do some predict ...');
//console.log(segmentor.predict('工信处女干事每月经过下属科室都要亲口交代24口交换机等技术性器件的安装工作'));
console.log(segmentor.predict('据埃航社交媒体官方账号消息'));
console.log(segmentor.predict('新华社北京5月31日电在“六一”国际儿童节即将到来之际，中共中央总书记、国家主席、中央军委主席习近平给大陈岛老垦荒队员的后代、浙江省台州市椒江区12名小学生回信，祝他们节日快乐，祝全国小朋友节日快乐'));



