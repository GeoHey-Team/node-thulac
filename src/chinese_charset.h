#pragma once
#include <cstring>
#include <iostream>
#include <iconv.h>
#include <vector>

namespace thulac{

class Chinese_Charset_Conv{
private:
    std::vector<std::string> codec;
    std::vector<iconv_t> convs;
    std::vector<iconv_t> invert_convs;
    int last_ind;

public :
    Chinese_Charset_Conv(){
        codec.push_back(std::string("utf8"));
        //codec.push_back(std::string("big5"));
        codec.push_back(std::string("gbk"));
        for(std::vector<std::string>::iterator it=this->codec.begin();it!=codec.end();++it){
            this->convs.push_back(iconv_open("utf8",it->c_str()));//iconv_open(to,from)
            this->invert_convs.push_back(iconv_open(it->c_str(),"utf8"));//iconv_open(to,from)
        };

        this->last_ind=0;
    };

    ~Chinese_Charset_Conv(){
        while(this->convs.size()){
            iconv_close(this->convs.back());
            this->convs.pop_back();
        };
        while(this->invert_convs.size()){
            iconv_close(this->invert_convs.back());
            this->invert_convs.pop_back();
        };
    };
    const std::string name(int ind){
        if(ind>=0 && ind<codec.size()){
            return codec[ind];
        };
        return NULL;
    };
    int invert_conv(char* i_str,const size_t i_len,char* o_str,size_t& o_left,int ind){
        char* in_buf=i_str;
        size_t in_left=i_len;
        char* out_buf=o_str;

        //iconv(this->invert_convs[ind],NULL,NULL,NULL,NULL);
        int rtn=iconv(this->invert_convs[ind],&in_buf,&in_left,&out_buf,&o_left);
        if(rtn==0){
            return ind;
        }
        return -1;
    };
    int conv(char* i_str,const size_t i_len,char* o_str,size_t& o_left,int pref=0){
        //int ind=this->last_ind;
        int ind=pref;
        do{
            //std::cout<<"try ind "<<ind<<" of "<<this->convs.size()<<"\n";
            char* in_buf=i_str;
            size_t in_left=i_len;
            char* out_buf=o_str;

            //iconv(this->convs[ind],NULL,NULL,NULL,NULL);
            int rtn=iconv(this->convs[ind],&in_buf,&in_left,&out_buf,&o_left);
            if(rtn==0){
                this->last_ind=ind;
                return ind;
            }

            if(ind==pref)ind=-1;
            ind++;
            if(ind==pref)ind++;
            if(ind==this->convs.size())break;

        }while(true);
        return -1;
    };

    void test(){
        const int BYTES_LEN=1000;//注意：字符串长度有限制
        char* s=new char[ BYTES_LEN];//输入缓存
        char* out=new char[ BYTES_LEN];//输出缓存
        //size_t in_left=strlen(s);

        while(true){
            std::cin.getline(s,BYTES_LEN);//读入输入
            if(std::cin.gcount()==0)break;//木有输入了，结束
            size_t in_left=std::cin.gcount()-1;//输入长度/要减去一个回车

            size_t out_left=BYTES_LEN;//调用下面函数的时候需要给出缓存大小
            int rtn=this->conv(s,in_left,out,out_left);//将gbk或者utf8转换成utf8
            //返回编码编号（从0开始）或者-1表示转码不成功

            if(rtn>=0){//如果成功
                int outlen=BYTES_LEN-out_left;//输出的bytes长度
                std::cout<<this->name(rtn)<<" 编码的字符:"<<std::string(out,outlen)<<"\n";//打印结果

                out_left=BYTES_LEN;
                int rtn2=this->invert_conv(out,(BYTES_LEN-out_left),s,out_left,rtn);//再转回原编码
                std::cout<<"转换回去的原编码字符:"<<std::string(s,out_left)<<"\n";

            };
        };
        
        delete[] s;
        delete[] out;
    };
};


}//end of namespace thulac
