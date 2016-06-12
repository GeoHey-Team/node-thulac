# node-thulac
node binding for [thulac](http://thulac.thunlp.org/)

## 依赖项
node >= 0.12.0

## 安装
```
npm install node-thulac
```

## 使用
```
var thulac = require('node-thulac');

var segmentor = new thulac.segmentor.Segmentor();

segmentor.loadModel({
    t2s: false,
    segOnly: false,
    filter: true,
    delimeter: '#',
    modelDir: './models/'
});

var segmentedList = segmentor.predict('新华社北京5月31日电在“六一”国际儿童节即将到来之际，中共中央总书记、国家主席、中央军委主席习近平给大陈岛老垦荒队员的后代、浙江省台州市椒江区12名小学生回信，祝他们节日快乐，祝全国小朋友节日快乐');

```

## API说明
loadModel 函数接受一个json对象，可以指定以下选项：

*t2s*                bool类型，指定是否将句子从繁体转化为简体

*segOnly*           bool类型，指定是否只进行分词，不进行词性标注

*delimeter*          string类型，设置词与词性间的分隔符，默认为下划线_

*filter*             bool类型，使用过滤器去除一些没有意义的词语，例如“可以”。

*userword*           string类型 设置用户词典，用户词典中的词会被打上uw标签。词典中每一个词一行，UTF8编码

*modelDir*           string类型 设置模型文件所在文件夹


predict 函数接受一个字符串，返回分词后的字符串。


