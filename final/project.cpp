#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <bitset>

// using namespace std;

#define int_init -1;
const bool debug_mode = true;

typedef struct bench{
    std::string bench_name;
    std::vector<std::vector<bool>> bench_data;
}Bench;

int vbtoi(std::vector<bool> index){
    std::string a;
    for(int i=0; i<index.size(); i++){
        if(index[i]){a.push_back('1');}
        else{a.push_back('0');}
    }
    return stoi(a, 0, 2);
}

std::string vbtos(std::vector<bool> index){
    std::string a;
    for(int i=0; i<index.size(); i++){
        if(index[i]){a.push_back('1');}
        else{a.push_back('0');}
    }
    return a;
}

std::vector<bool> trim_offset(std::vector<bool> address, int offset_bit_count){
    for(int i=0; i<offset_bit_count; i++){
        address.pop_back();
    }
    return address;
}

std::vector<bool> select_address(std::vector<bool> address, std::vector<int> indexing_bits){
    int index_len = indexing_bits.size();
    int index_pivot = address.size() - 1;
    std::vector<bool> index;
    if(index_len > 0 && index_pivot >= 0){
        for(int i=index_len-1; i>=0; i--){
            int sel_idx = index_pivot - indexing_bits[i];
            if(sel_idx >= 0){
                index.push_back(address[sel_idx]);
            }
        }
    }else{
        std::cout << "No Indexing Bits" << std::endl;
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

bool write_log(std::string str, bool debug_mode){
    if(debug_mode){
        std::cout << str << std::endl;
    }
}

Bench read_bench(std::fstream *file){
    std::string line_buffer;
    // std::vector<std::vector<bool>> bench_data;
    std::vector<bool> tmp;
    Bench b;
    
    line_buffer = read_stream(file, debug_mode);
    b.bench_name = read_stream(file, debug_mode);
    line_buffer = read_stream(file, debug_mode);
    for(int i=0; line_buffer != ".end"; i++){
        for(int j=0; j<line_buffer.size(); j++){
            tmp.push_back(line_buffer[j] == '1');
        }
        b.bench_data.push_back(tmp);
        tmp.clear();
        line_buffer = read_stream(file, debug_mode);
    }   
    return b;
}

bool show_bench(Bench bch){
    std::cout << "\n.benchmark " << bch.bench_name << std::endl;
    for(int i=0; i<bch.bench_data.size(); i++){
        for(int j=0; j<bch.bench_data[i].size(); j++){
            std::cout << bch.bench_data[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << ".end" << std::endl;
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
        if(this->associativity == 0){std::cout << "ERROR Associativity is 0" << std::endl; system("pause"); return ;}
        for (int i = 0; i < this->cache_size; i++){
            cache_entry.push_back(0);
            valid_bits.push_back(false);
            nru_table.push_back(true);
        }
        
    }

    bool setup(){
        // LSB
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
        std::vector<bool> selected_address = select_address(target_vb, this->indexing_bits);
        int target = vbtoi(selected_address);
        int trim_addr = vbtoi(trim_offset(target_vb, this->offset_bit_count));
        // int target = vbtoi(trim_offset(target_vb, this->offset_bit_count));
        write_log(std::to_string(target) + " <-- " + vbtos(selected_address) + " <--Sel " + std::to_string(trim_addr) + " <--Trim " + vbtos(target_vb), debug_mode);
        if(this->cache_sets > 0){
            int set_index  = target % this->cache_sets;
            write_log(std::to_string(set_index) + " Set Index", debug_mode);
            for(int i=this->associativity * set_index; i<this->associativity * (set_index + 1); i++){
                if(this->cache_entry[i] == trim_addr && this->valid_bits[i]){
                    nru_hit_policy(i);
                    write_log("HIT Way " + std::to_string(i - this->associativity * set_index), debug_mode);
                    return true;
                }
            }
            write_log("MISS", debug_mode);
            return false;
        }else{
            for(int i=0; i<this->cache_size; i++){
                if(this->cache_entry[i] == trim_addr && this->valid_bits[i]){
                    nru_hit_policy(i);
                    write_log("HIT Entry " + std::to_string(i), debug_mode);
                    return true;
                }
            }
            write_log("MISS", debug_mode);
            return false;
        }
        
    }

    bool replace(std::vector<bool> target_vb){
        // NRU Policy
        std::vector<bool> selected_address = select_address(target_vb, this->indexing_bits);
        int target = vbtoi(selected_address);
        int trim_addr = vbtoi(trim_offset(target_vb, this->offset_bit_count));
        bool re = false;
        if(this->cache_sets > 0){
            int set_index  = target % this->cache_sets;
            int start_idx = this->associativity * set_index;
            int end_idx = this->associativity * (set_index + 1);
            for(int i=start_idx; i<end_idx; i++){
                if(this->nru_table[i] == true){
                    this->nru_table[i] = false;
                    this->valid_bits[i] = true;
                    this->cache_entry[i] = trim_addr;
                    write_log("REPLACE Set " + std::to_string(set_index) + " Way " + std::to_string(i - start_idx), debug_mode);
                    return true;
                }
            }
            for(int i=start_idx; i<end_idx; i++){
                this->nru_table[i] = true;
            }
            this->nru_table[start_idx] = false;
            this->valid_bits[start_idx] = true;
            this->cache_entry[start_idx] = trim_addr;
            write_log("REPLACE Set " + std::to_string(set_index) + " Way 0: FULL", debug_mode);
            return true;
        }else{
            for(int i=0; i<this->cache_size; i++){
                if(this->nru_table[i] == true){
                    this->nru_table[i] = false;
                    this->valid_bits[i] = true;
                    this->cache_entry[i] = trim_addr;
                    write_log("REPLACE Entry " + std::to_string(i) + " FULL", debug_mode);
                    return true;
                }
            }
            for(int i=0; i<this->cache_size; i++){
                this->nru_table[i] = true;
            }
            this->nru_table[0] = false;
            this->valid_bits[0] = true;
            this->cache_entry[0] = trim_addr;
            write_log("REPLACE Entry 0 FULL", debug_mode);
            return true;
        }
        return false;
    }

    bool fetch(std::vector<bool> target_address){
        bool is_hit = hit(target_address);
        if(!is_hit){
            replace(target_address);
        }
        return is_hit;
    }

    bool run_bench(Bench b){

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

    float run_bench(Bench b, std::fstream *file, bool is_output){
        if(file == NULL && is_output){std::cout << "ERROR: No specify output file" << std::endl; return 0;}
        float hit_rate = 0;
        int hit_times = 0;
        int bench_size = b.bench_data.size();
        write_file(file, ".benchmark " + b.bench_name, true, debug_mode);
        for(int i=0; i<bench_size; i++){
            bool is_hit = fetch(b.bench_data[i]);
            if(is_hit){hit_times++;}
            if(is_output){write_file(file, vbtos(b.bench_data[i]) + (is_hit? " hit":" miss"), true, debug_mode);}
            if(debug_mode){show_cache();}
        }
        if(is_output){write_file(file, ".end", true, debug_mode);}
        write_log("Hit Rate: " + std::to_string((float)(hit_times)/(float)(bench_size)), debug_mode);
        return (float)(hit_times)/(float)(bench_size);
    }

    bool show_cache(){
        std::cout << "\nEntry\tVali\tNRU" << std::endl;
        for(int i=0; i<cache_entry.size(); i++){
            if(this->associativity > 0){
                if(i % this->associativity == 0){std::cout << "------------------------" << std::endl;}
            }
            
            std::cout << this->cache_entry[i] << "\t" << this->valid_bits[i] << "\t" << this->nru_table[i] << std::endl;
        }
        return true;
    }

    bool clear_cache(){
        this->cache_entry.clear();
        this->valid_bits.clear();
        this->nru_table.clear();
    }

    bool clear_all(){
        this->address_bits = 0;
        this->block_size = 0;
        this->cache_sets = 0;
        this->associativity = 0;
        this->cache_size = 0;

        this->offset_bit_count = 0;
        this->indexing_bit_count = 0;

        this->indexing_bits.clear();
        this->cache_entry.clear();
        this->valid_bits.clear();
        this->nru_table.clear();
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
    // std::cout << "Hello C++ Again" << std::endl;

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

    Bench bch = read_bench(&lst_file);
    
    // c.show_cache();
    float hit_rate = c.run_bench(bch, &rpt_file, true);
    // for(int i=0; i<bch.bench_data.size(); i++){
    //     c.fetch(bch.bench_data[i]);
    //     c.show_cache();
    // }
    
    return 0;
}