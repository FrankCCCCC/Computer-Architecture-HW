#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <algorithm>

// using namespace std;

#define int_init -1;
const bool debug_mode = false;
const bool sel_idx_debug = false;

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
        std::cout << "Read File Stream is NULL" << std::endl;
        return "";
    }
    std::string str;
    (*file) >> str;

    if(is_show){std::cout << str;}
    return str;
}

bool write_file(std::fstream *file, std::string str, bool is_newline, bool is_show){
    if (!*file){
        std::cout << "Write File Stream is NULL" << std::endl;
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
        this->is_setup = false;
        this->offset_bit_count = log2(block_size);
        this->indexing_bit_count = log2(cache_sets);
        if(this->associativity == 0){std::cout << "ERROR Associativity is 0" << std::endl; system("pause"); return ;}
        for (int i = 0; i < this->cache_size; i++){
            cache_entry.push_back(0);
            valid_bits.push_back(false);
            nru_table.push_back(true);
        }
        
    }

    bool setup(std::vector<int> indexing_bits){
        // Manual Indexing Bits
        if(indexing_bits.size() != this->indexing_bit_count){
            std::cout << "ERROR: Input Vector indexing_bits.size() Isn't Equal to this->indexing_bit_count" << std::endl;
            return false;
        }
        std::sort(indexing_bits.begin(), indexing_bits.end());
        this->indexing_bits = indexing_bits;
        this->is_setup = true;

        return true;
    }

    bool setup(){
        // LSB
        // this->offset_bit_count = log2(block_size);
        // this->indexing_bit_count = log2(cache_sets);
        for(int i=0;i<this->indexing_bit_count;i++){
            this->indexing_bits.push_back(this->offset_bit_count + i);
        }
        this->is_setup = true;
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

    bool output_cache_info(std::fstream *file){
        if(!is_setup){
            std::cout << "ERROR: Haven't setup" << std::endl;
            return false;
        }
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
            write_file(file, " " + std::to_string(this->indexing_bits[i]), false, debug_mode);
        }
        write_file(file, "\n", true, debug_mode);

        return true;
    }

    float run_bench(Bench b, std::fstream *file, bool is_output){
        if(!is_setup){
            std::cout << "ERROR: Haven't setup" << std::endl;
            return false;
        }
        if(file == NULL && is_output){std::cout << "ERROR: No specify output file" << std::endl; return 0;}
        float hit_rate = 0;
        int hit_times = 0;
        int miss_times = 0;
        int bench_size = b.bench_data.size();
        if(is_output){write_file(file, ".benchmark " + b.bench_name, true, debug_mode);}
        for(int i=0; i<bench_size; i++){
            bool is_hit = fetch(b.bench_data[i]);
            if(is_hit){hit_times++;}else{miss_times++;}
            if(is_output){write_file(file, vbtos(b.bench_data[i]) + (is_hit? " hit":" miss"), true, debug_mode);}
            if(debug_mode){show_cache();}
        }
        if(is_output){
            write_file(file, ".end\n", true, debug_mode);
            write_file(file, "Total cache miss count: " + std::to_string(miss_times), true, debug_mode);
        }
        write_log("Hit Rate: " + std::to_string((float)(hit_times)/(float)(bench_size)), debug_mode);
        return (float)(hit_times)/(float)(bench_size);
    }

    bool show_cache(){
        if(!is_setup){
            std::cout << "ERROR: Haven't setup" << std::endl;
            return false;
        }
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

    // private:
    int address_bits;
    int block_size;
    int cache_sets;
    int associativity;
    int cache_size;
    bool is_setup;

    int offset_bit_count;
    int indexing_bit_count;
    std::vector<int> indexing_bits;
    std::vector<int> cache_entry;
    std::vector<bool> valid_bits;
    std::vector<bool> nru_table;
};

typedef struct bit_set{
    std::vector<int> bits;
    std::vector<int> samples;
    float score;
}Bit_Set;

float quality_score(int bench_size, int true_times){
    return true_times >= bench_size? (float)(bench_size - true_times) / (float)(true_times) : (float)(true_times) / (float)(bench_size - true_times);
}

float correlation_score(int a_sample_size, int b_smaple_size, int inter_sample_size){
    int diff = a_sample_size + b_smaple_size - (2 * inter_sample_size);
    if(inter_sample_size >= diff){
        if(inter_sample_size == 0){
            return 0;
        }else{
            return (float)(diff) / (float)(inter_sample_size);
        }
    }else{
        if(diff == 0){
            return 0;
        }else{
            return (float)(inter_sample_size) / (float)(diff);
        }
    }
    // return inter_sample_size >= diff? (float)(diff) / (float)(inter_sample_size) : (float)(inter_sample_size) / (float)(diff);
}

Bit_Set intersect(Bit_Set a, Bit_Set b){
    
    int ab_bits_size_sum = a.bits.size() + b.bits.size();
    int ab_samples_size_sum = a.samples.size() + b.samples.size();
    std::vector<int> ab_bits_v(ab_bits_size_sum);
    std::vector<int> ab_samples_v(ab_samples_size_sum);
    Bit_Set inter{ab_bits_v, ab_samples_v, 0};
    
    sort(a.bits.begin(), a.bits.end());
    
    sort(b.bits.begin(), b.bits.end());
    
    merge(a.bits.begin(), a.bits.end(), b.bits.begin(), b.bits.end(), inter.bits.begin());
    
    sort(a.samples.begin(), a.samples.end());
    
    sort(b.samples.begin(), b.samples.end());
    std::vector<int>::iterator it_s = set_intersection(a.samples.begin(), a.samples.end(), b.samples.begin(), b.samples.end(), inter.samples.begin());
    inter.samples.resize(it_s - inter.samples.begin());

    int b_sample_size = b.samples.size();
    int a_sample_size = a.samples.size();
    int inter_sample_size = inter.samples.size();
    write_log(std::to_string(a_sample_size) + " " + std::to_string(b_sample_size) + " " + std::to_string(inter_sample_size), sel_idx_debug);
    inter.score = correlation_score(a_sample_size, b_sample_size, inter_sample_size);
    write_log(std::to_string(inter.score), sel_idx_debug);

    return inter;
}

Bit_Set best_bit_set(std::vector<Bit_Set> indexing_bit_set, int indexing_bit_count){
    int ibs_size = indexing_bit_set.size();
    std::vector<Bit_Set> best_indexing_bit_set = indexing_bit_set;
    for(int ibc=0; ibc<indexing_bit_count-1; ibc++){
        write_log("Select Best bit at " + std::to_string(ibc), sel_idx_debug);
        int best_ibs_size = best_indexing_bit_set.size();
        std::vector<Bit_Set> temp;
        for(int i=0; i<best_ibs_size; i++){
            for(int j=0; j<ibs_size; j++){
                if(std::find(best_indexing_bit_set[i].bits.begin(), best_indexing_bit_set[i].bits.end(), indexing_bit_set[j].bits[0]) == best_indexing_bit_set[i].bits.end()){
                    write_log("Select Best bit between " + std::to_string(best_indexing_bit_set[i].bits[0]) + " " + std::to_string(indexing_bit_set[j].bits[0]), sel_idx_debug);
                    temp.push_back(intersect(best_indexing_bit_set[i], indexing_bit_set[j]));
                }
            }
        }

        // Sort & Deduce
        sort(temp.begin(), temp.end(), [](const Bit_Set &a, const Bit_Set &b){return a.score > b.score;});
        // std::cout << temp[0].score << " " << temp[1].score << " " << temp[2].score << " " << temp[3].score << " " << temp[4].score << std::endl;
        temp.erase(temp.begin() + ibs_size * 3, temp.end());
        // std::cout << temp[0].score << " " << temp[1].score << " " << temp[2].score << " " << temp[3].score << " " << temp[4].score << std::endl;
        best_indexing_bit_set = temp;
    }
    return best_indexing_bit_set[0];
}

std::vector<int> select_indexing_bits(Bench bch, Cache c){
    std::vector<Bit_Set> indexing_bit_sets;

    int index_bit_range = c.address_bits - c.offset_bit_count;
    int index_bit_start = c.offset_bit_count;
    int index_bit_end = c.address_bits - 1;
    int bench_size = bch.bench_data.size();
    write_log("Cache indexing bit range " + std::to_string(index_bit_range) + " from " + std::to_string(index_bit_start) + " to " + std::to_string(index_bit_end) + " in bench size " + std::to_string(bench_size), sel_idx_debug);

    for(int i=index_bit_start; i<=index_bit_end; i++){
        std::vector<int> tv({i});
        std::vector<int> sa;
        Bit_Set bs{tv, sa, 0};
        indexing_bit_sets.push_back(bs);
        write_log("Set " + std::to_string(bs.bits[0]) + " score: " + std::to_string(bs.score), sel_idx_debug);
    }
    for(int j=0; j<index_bit_range; j++){
        int true_times = 0;
        write_log("Set " + std::to_string(indexing_bit_sets[j].bits[0]) + " score: " + std::to_string(indexing_bit_sets[j].score), sel_idx_debug);
        int l_bit = c.address_bits - indexing_bit_sets[j].bits[0] - 1;
        for(int i=0; i<bench_size; i++){
            if(bch.bench_data[i][l_bit]){
                indexing_bit_sets[j].samples.push_back(i);
                true_times++;
                write_log(std::to_string(i), sel_idx_debug);
            }
        }
        indexing_bit_sets[j].score = quality_score(bench_size, true_times);
        write_log("Set " + std::to_string(indexing_bit_sets[j].bits[0]) + " score: " + std::to_string(indexing_bit_sets[j].score), sel_idx_debug);
    }
    Bit_Set bit_set_best = best_bit_set(indexing_bit_sets, c.indexing_bit_count);

    // c.setup(indexing_bits);
    return bit_set_best.bits;
}

int main(int argc, char** argv){
    std::cout << "Getting Start..." << std::endl;
    std::string org_file_name = argv[1];
    std::string lst_file_name = argv[2];
    std::string rpt_file_name = argv[3];

    std::fstream org_file;
    std::fstream lst_file;
    std::fstream rpt_file;

    org_file.open(org_file_name, std::ios::in);
    lst_file.open(lst_file_name, std::ios::in);
    rpt_file.open(rpt_file_name, std::ios::out);

    if(!org_file){std::cout << "Cannot Open file " << org_file_name << std::endl; return 0;}
    if(!lst_file){std::cout << "Cannot Open file " << lst_file_name << std::endl; return 0;}
    if(!rpt_file){std::cout << "Cannot Open file " << rpt_file_name << std::endl; return 0;}

    Bench bch = read_bench(&lst_file);

    Cache c(&org_file);
    std::vector<int> best_index = select_indexing_bits(bch, c);
    c.setup(best_index);
    c.output_cache_info(&rpt_file);
    // c.show_cache();
    float hit_rate = c.run_bench(bch, &rpt_file, true);

    org_file.close();
    lst_file.close();
    rpt_file.close();

    // org_file.open(org_file_name, std::ios::in);
    // if(!org_file){std::cout << "Cannot Open file " << org_file_name << std::endl; return 0;}

    // Cache c0(&org_file);
    // c0.setup();
    // float hit_rate0 = c0.run_bench(bch, &rpt_file, false);

    // float higher = hit_rate - hit_rate0;

    // if(hit_rate0 > hit_rate){
    //     std::cout << "0 ";
    //     rpt_file.open(rpt_file_name, std::ios::out);
    //     if(!rpt_file){std::cout << "Cannot Open file " << rpt_file_name << std::endl;}
    //     higher = hit_rate0 - hit_rate;
    //     c0.output_cache_info(&rpt_file);
    //     hit_rate = c0.run_bench(bch, &rpt_file, true);
    //     rpt_file.close();
    // }

    // org_file.close();
    
    // std::cout << "Hit Rate: " << std::to_string(hit_rate) << " +" << std::to_string(higher) << std::endl;
    std::cout << "Hit Rate: " << std::to_string(hit_rate) << std::endl;
    
    return 0;
}