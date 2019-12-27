#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <bitset>

// using namespace std;

#define int_init -1;
const bool debug_mode = true;

int vbtoi(std::vector<bool> index){
    std::string a;
    for(int i=0; i<index.size(); i++){
        if(index[i]){a.push_back('1');}
        else{a.push_back('0');}
    }
    return stoi(a, 0, 2);
}

std::vector<bool> trim_offset(std::vector<bool> address, int offset_bit_count){
    for(int i=0; i<offset_bit_count; i++){
        address.pop_back();
    }
    return address;
}

std::vector<bool> select_address(std::vector<bool> address, std::vector<int> indexing_bits){
    int index_len = indexing_bits.size();
    std::vector<bool> index;
    if(index_len > 0){
        for(int i=0; i<index_len; i++){
            index.push_back(address[indexing_bits[i]]);
        }
    }
    return index;
}

std::string read_stream(std::fstream *file, bool is_show){
    if (!*file){
        std::cout << "File Stream is NULL" << std::endl;
        return "";
    }
    std::string str;
    (*file) >> str;

    if(is_show){std::cout << str;}
    return str;
}

bool write_file(std::fstream *file, std::string str, bool is_newline, bool is_show){
    if (!*file){
        std::cout << "File Stream is NULL" << std::endl;
        return false;
    }
    (*file) << str;
    if(is_newline){(*file) << std::endl;}
    if(is_show){
        std::cout << str;
        if(is_newline){std::cout << std::endl;}
    }
    return true;
}

std::vector<std::vector<bool>> read_bench(std::fstream *file){
    std::string line_buffer;
    std::vector<std::vector<bool>> bench_data;
    std::vector<bool> tmp;
    line_buffer = read_stream(file, debug_mode);
    line_buffer = read_stream(file, debug_mode);
    line_buffer = read_stream(file, debug_mode);
    for(int i=0; line_buffer != ".end"; i++){
        for(int j=0; j<line_buffer.size(); j++){
            tmp.push_back(line_buffer[j] == '1');
        }
        bench_data.push_back(tmp);
        tmp.clear();
        line_buffer = read_stream(file, debug_mode);
    }
    return bench_data;
}

class Cache{
    public:
    Cache(std::fstream *org_file){
        std::string line_buffer;
        line_buffer = read_stream(org_file, debug_mode);
        line_buffer = read_stream(org_file, debug_mode);
        this->address_bits = stoi(line_buffer);

        line_buffer = read_stream(org_file, debug_mode);
        line_buffer = read_stream(org_file, debug_mode);
        this->block_size = stoi(line_buffer);

        line_buffer = read_stream(org_file, debug_mode);
        line_buffer = read_stream(org_file, debug_mode);
        this->cache_sets = stoi(line_buffer);

        line_buffer = read_stream(org_file, debug_mode);
        line_buffer = read_stream(org_file, debug_mode);
        this->associativity = stoi(line_buffer);

        this->cache_size = this->cache_sets * this->associativity;
        for (int i = 0; i < this->cache_size; i++){
            valid_bits.push_back(false);
            nru_table.push_back(true);
        }
        
    }

    bool setup(){
        this->offset_bit_count = log2(block_size);
        this->indexing_bit_count = log2(cache_sets);
        for(int i=0;i<this->indexing_bit_count;i++){
            this->indexing_bits.push_back(this->offset_bit_count + i);
        }
        return true;
    }

    bool nru_hit_policy(int idx){
        this->nru_table[idx] = false;
        return true;
    }

    bool hit(std::vector<bool> target_vb){
        int target = vbtoi(trim_offset(target_vb, this->offset_bit_count));
        if(this->cache_sets > 0){
            int set_index  = target % this->cache_sets;
            for(int i=this->associativity * set_index; i<this->associativity * (set_index + 1); i++){
                if(this->cache_entry[i] == target && this->valid_bits[i]){nru_hit_policy(i); return true;}
            }
            return false;
        }else{
            for(int i=0; i<this->cache_size; i++){
                if(this->cache_entry[i] == target && this->valid_bits[i]){nru_hit_policy(i); return true;}
            }
            return false;
        }
        
    }

    bool replace(std::vector<bool> target_vb){
        // NRU Policy
        int target = vbtoi(trim_offset(target_vb, this->offset_bit_count));
        bool re = false;
        if(this->cache_sets > 0){
            int set_index  = target % this->cache_sets;
            int start_idx = this->associativity * set_index;
            int end_idx = this->associativity * (set_index + 1);
            for(int i=start_idx; i<end_idx; i++){
                if(this->nru_table[i] == true){
                    this->nru_table[i] = false;
                    this->valid_bits[i] = true;
                    this->cache_entry[i] = target;
                    return true;
                }
            }
            for(int i=start_idx; i<end_idx; i++){
                this->nru_table[i] = true;
            }
            this->nru_table[start_idx] = false;
            this->valid_bits[start_idx] = true;
            this->cache_entry[start_idx] = target;
            return true;
        }else{
            for(int i=0; i<this->cache_size; i++){
                if(this->nru_table[i] == true){
                    this->nru_table[i] = false;
                    this->valid_bits[i] = true;
                    this->cache_entry[i] = target;
                    return true;
                }
            }
            for(int i=0; i<this->cache_size; i++){
                this->nru_table[i] = true;
            }
            this->nru_table[0] = false;
            this->valid_bits[0] = true;
            this->cache_entry[0] = target;
            return true;
        }
        return false;
    }

    bool fetch(std::vector<bool> target_address){
        bool is_hit = hit(target_address);
        if(!is_hit){
            return replace(target_address);
        }else{return is_hit;}
    }

    bool output_cache_info(std::fstream *file){
        if (!*file){
            std::cout << "File Stream is NULL" << std::endl;
            return false;
        }
        write_file(file, "Address bits: " + std::to_string(this->address_bits), true, debug_mode);
        write_file(file, "Block size: " + std::to_string(this->block_size), true, debug_mode);
        write_file(file, "Cache sets: " + std::to_string(this->cache_sets), true, debug_mode);
        write_file(file, "Associativity: " + std::to_string(this->associativity), true, debug_mode);

        write_file(file, "", true, debug_mode);

        
        write_file(file, "Offset bit count: " + std::to_string(this->offset_bit_count), true, debug_mode);
        write_file(file, "Indexing bit count: " + std::to_string(this->indexing_bit_count), true, debug_mode);
        write_file(file, "Indexing bits:", false, debug_mode);
        for(int i=0;i<indexing_bit_count;i++){
            write_file(file, " " + std::to_string(offset_bit_count+i), false, debug_mode);
        }
        write_file(file, "\n", true, debug_mode);

        return true;
    }

    private:
    int address_bits;
    int block_size;
    int cache_sets;
    int associativity;
    int cache_size;

    int offset_bit_count;
    int indexing_bit_count;
    std::vector<int> indexing_bits;
    std::vector<int> cache_entry;
    std::vector<bool> valid_bits;
    std::vector<bool> nru_table;
};



int main(int argc, char** argv){
    std::cout << "Hello C++ Again" << std::endl;

    std::string org_file_name = argv[1];
    std::string lst_file_name = argv[2];
    std::string rpt_file_name = argv[3];

    std::fstream org_file;
    std::fstream lst_file;
    std::fstream rpt_file;

    org_file.open(org_file_name, std::ios::in);
    lst_file.open(lst_file_name, std::ios::in);
    rpt_file.open(rpt_file_name, std::ios::out);

    if(!org_file){std::cout << "Cannot Open file " << org_file_name << std::endl;}
    if(!lst_file){std::cout << "Cannot Open file " << lst_file_name << std::endl;}
    if(!rpt_file){std::cout << "Cannot Open file " << rpt_file_name << std::endl;}

    std::string line_buffer;

    // std::getline(org_file, line_buffer);
    Cache c(&org_file);
    c.setup();
    c.output_cache_info(&rpt_file);

    std::vector<std::vector<bool>> bench_data = read_bench(&lst_file);
    // for(int i=0; bench_data.size(); i++){
    //     for(int j=0; j<bench_data[i].size(); j++){
    //         std::cout << bench_data[i][j];
    //     }
    //     std::cout << std::endl;
    // }
    std::cout << std::endl << vbtoi(trim_offset(bench_data[1], 2));
    std::cout << std::endl << vbtoi(bench_data[1]);

    return 0;
}