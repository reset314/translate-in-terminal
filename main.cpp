#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstdint>
#include <cctype>
#include "ShardLoader.h"

const int MAX_DIST = 8;
ShardLoader loader;

std::string get_data_path() {
    // 获取可执行文件所在目录（Linux）
    std::error_code ec;
    auto exe_path = std::filesystem::read_symlink("/proc/self/exe", ec);
    if (ec) return "./data";
    return (exe_path.parent_path() / "data").string();
}

bool load_(std::string bin_name){
    std::string bin_path = get_data_path() + '/' + bin_name + ".bin";
    if(!loader.load(bin_path)){
        std::cout << "can find bin file.\n找不到bin文件" << std::endl;
        return false;
    }
    return true;
}

void find_(const std::string& input_word){
    std::vector<ShardLoader::Result> results = loader.search(input_word, MAX_DIST);
    if (results.size() == 1){
        std::cout << results[0].translate << std::endl;
                //   << results[0].phonetic << std::endl;
                //   todo:更多的字段输出,注意某些字段需要特殊处理
    }
    for(int i = 0; i < 10; ++i){
        if(i >= results.size()) break;
        std::cout << results[i].word << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]){
    if (argc < 2) {
        std::cout << "Usage: translate <word>\n用法: translate <单词>" << std::endl;
        return 1;
    }
    std::string input_word = argv[1];
    
    if (input_word.size() < 2){
        if (std::isalpha(input_word[0])){
            if(!load_("other")) return 1;
            find_(input_word);
            return 0;
        } else {
            std::cout << "need more input\n需要更多输入" << std::endl;
            return 1;
        }
    } else {
        if (std::isalpha(input_word[0]) && std::isalpha(input_word[1])){
            if(!load_(input_word.substr(0,2))) return 1;
            find_(input_word);
            return 0;
        } else {
            if(!load_("other")) return 1;
            find_(input_word);
            return 0;
        }
    }
}
