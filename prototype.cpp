#include<vector>
#include<string>
#include<iostream>
#include<algorithm>
#include<map>
#include<fstream>
#include<chrono>

//TODO
//tokenizacia
//permutacie
//cisla na n-nasobok zatvorky

using namespace std;


struct Node{
    map<char,Node*> children;
    Node* parent;
    char c;
    int freq = 0;
    int depth;
    Node(char ch,Node* p = nullptr,int f = 0){
        if(p == nullptr) depth = 0;
        else depth = p->depth + 1;
        parent = p;
        c = ch;
        freq = f;
    }
    Node* add_child(char ch){
        if(children.find(ch) == children.end()) children[ch] = new Node(ch,this);
        return children[ch];
    }
};

typedef vector<Node*> type_OUTPUT;

struct Trie{
    Node* root;
    vector<string> morse{"","et","ianm","surwdkgo","hvflpjbxcyzq"};
    map<char,int> matching{{'(',1},{')',1},{'{',2},{'}',2},{'[',3},{']',3},{'<',4},{'>',4}};
    vector<void(Trie::*)(string,type_OUTPUT&)> functions;
    Trie(){
        root = new Node(':');
        functions = {&this->from_set,&this->permute};
    }
    void add_string(string s,int f, Node* cur = nullptr){
        if(find(s)) return;
        if(cur == nullptr) cur = root;
        for(int ind = 0;ind < s.size();ind++){
            cur = cur->add_child(s[ind]);
        }
        cur->freq += f;
    }
    bool find(string word){
        Node* cur = root;
        int ind = 0;
        while(ind != word.size() || cur != nullptr){
            if(cur->children.find(word[ind]) != cur->children.end()) cur = cur->children[word[ind++]];
            else return 0;
            if(cur->c == '$') return 1;
        }
        return 0;
    }
    void o_dfs_match_pattern(string pattern,vector<pair<string,int>>& out,int ind = 0,Node* cur = nullptr){
        if(cur == nullptr) cur = root;
        if(cur->c == '$' && ind == pattern.size()){ //match
            int f = cur->freq;
            string ans = "";
            cur = cur->parent;
            while(cur->parent != nullptr){
                ans += cur->c;
                cur = cur->parent;
            }
            reverse(ans.begin(),ans.end());
            out.push_back({ans,f});
            return;
        }
        if(ind == pattern.size()) return; //no match
        if(pattern[ind] == '.'){ //resolve wildcard character
            for(auto it = cur->children.begin();it != cur->children.end();it++){
                o_dfs_match_pattern(pattern,out,ind + 1, it->second);
            }
        }
        if(cur->children.find(pattern[ind]) != cur->children.end()){
            o_dfs_match_pattern(pattern,out,ind + 1,cur->children[pattern[ind]]);    
        }
    }
    void advance(char next,type_OUTPUT& cur){
        type_OUTPUT out;
        for(auto i : cur){
            if(next == '.'){
                for(auto j : i->children) out.push_back(j.second);
            }else if(i->children.find(next) != i->children.end()){
                out.push_back(i->children.find(next)->second);
            }
        }
        cur = out;
    }
    void from_set(string pattern, type_OUTPUT& cur){
        type_OUTPUT out;
        for(char i : pattern){
            for(auto j : cur){
                if(j->children.find(i) != j->children.end()) out.push_back(j->children.find(i)->second);
            }
        }
        cur = out;
    }
    void permute(string pattern, type_OUTPUT&cur){
    }
    string get_word(Node* cur){
        string out(cur->depth+1,' ');
        while(cur != nullptr){
            out[cur->depth] = cur->c;
            cur = cur->parent;
        }
        return out.substr(1,out.size()-2);
    }
    void match_pattern(string pattern,type_OUTPUT& out){ //TODO tokenizacia
        out.push_back(this->root);
        int mode = 0;
        string tmp;
        for(char i : pattern){
            if(mode){
                if(matching.find(i) != matching.end()){
                    (this->*functions[mode-1])(tmp,out);
                    mode = 0;
                    tmp = "";
                }else{
                    tmp += i;
                }
            }else if((i<='z' && i >= 'a') || i == '.' || i == '$'){
                advance(i,out);
            }else if(i <= '4' && i >= '1'){
                from_set(morse[i-'0'],out);
            }else if(matching.find(i) != matching.end()){
                mode = matching[i];
            }
        }
    }
    void print_words(Node* nv = nullptr,string cur_str = ""){
        if(nv == nullptr) nv = root;
        cur_str.push_back(nv->c);
        if(nv->children.size() == 0){
            cout<<cur_str<<endl;
            return;
        }
        for(auto it = nv->children.begin();it != nv->children.end();it++){
            print_words(it->second,cur_str);
        }
        cur_str.pop_back();
        return;
    }
    long long number_of_nodes(Node* nv = nullptr){
        if(nv == nullptr) nv = root;
        if(nv->children.size() == 0) return 1;
        long long ans = 0;
        for(auto it = nv->children.begin();it != nv->children.end();it++){
            ans += number_of_nodes(it->second);
        }
        return ans;
    }
    void serialize(ostream &os, Node* nv = nullptr){
        if(nv == nullptr) nv = root;
        os<<nv->c<<" ";
        if(nv->c == '$') os<<nv->freq<<" ";
        for(auto it = nv->children.begin();it != nv->children.end();it++){
            serialize(os,it->second);
        }
        os<<"# ";
    }
    void serialize(string out){
        ofstream os(out);
        serialize(os);
        os<<endl;
        os.close();
    }
    void deserialize(string dump){
        auto begin = chrono::high_resolution_clock::now();
        ifstream in;
        in.open(dump);
        while(!in.is_open()){
            cout<<"Could not open wordlist. Enter path to trie.dump file: ";
            string path;
            if(path == "q") return;
            cin>>path;
            in.open(path);
        }
        Node* nv = root;
        char c;in>>c;
        int freq;
        while(nv != nullptr){
            in>>c;
            if(c == '#') nv = nv->parent;
            else{
                nv = nv->add_child(c);
                if(c == '$'){in>>freq;nv->freq = freq;}
            }
        }
        auto end = chrono::high_resolution_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(end-begin);
        cout<<"Wordlist loaded succesfully in "<<elapsed.count()<<"ms!"<<endl;
        in.close();
    }
};

Trie tr = Trie();

bool comp(pair<string,int> a,pair<string,int> b){
    return a.second > b.second || (a.second == b.second && a.first > b.first);
}
bool comp1(Node* a,Node* b){
    return a->freq > b->freq;
}

void benchmark(string a){
    auto begin = chrono::high_resolution_clock::now();
    type_OUTPUT ans;
    tr.match_pattern(a,ans);
    for(auto i : ans) tr.get_word(i);
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end-begin);
    cout<<"New time: "<<elapsed.count()<<"ms"<<endl;
    
    begin = chrono::high_resolution_clock::now();
    vector<pair<string,int>> o_ans;
    tr.o_dfs_match_pattern(a,o_ans);
    end = chrono::high_resolution_clock::now();
    elapsed = chrono::duration_cast<chrono::milliseconds>(end-begin);
    cout<<"Old time: "<<elapsed.count()<<"ms"<<endl;
}

void manual_inp(){
    char id;
    string co;
    while(1){
        cin>>id;
        if(id == '+'){
            int f;
            cin>>co>>f;
            co += '$';
            tr.add_string(co,f);
        }else if(id == '?'){
            cin>>co;
            co += '$';
            type_OUTPUT ans;
            tr.match_pattern(co,ans);
            sort(ans.begin(),ans.end(),comp1);
            int ind = 0;
            while(ind < ans.size()){
                for(int cnt = 0;cnt <= 31 && ind < ans.size();cnt++,ind++){
                    cout<<tr.get_word(ans[ind])<<endl;
                }
                if(ind == ans.size()) break;
                cout<<"Show more results? [y/n] ";
                string a;cin>>a;
                if(a != "y" && a != "Y") break;
            }
        }else if(id == 'd') tr.print_words();
        else if(id == 'b'){
            string a;cin>>a;
            benchmark(a);
        }
        else break;
    };
}

void load_wordlist(string file){
    auto begin = chrono::high_resolution_clock::now();
    ifstream wl;
    wl.open(file);
    if(!wl.is_open()){
        cout<<"Can not open wordlist"<<endl;
        return;
    }
    string line;
    while(getline(wl,line)){
        string word = line.substr(0,line.find(' '));
        int freq = stoi(line.substr(line.find(' ') + 1,line.size() - 1 - word.size()));
        word += '$';
        tr.add_string(word,freq);
    }
    wl.close();
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end-begin);
    cout<<"Wordlist loaded in "<<elapsed.count()<<"ms"<<endl;
}

int main(){
    //load_wordlist("sk_parsed.txt");
    tr.deserialize("trie.dump");
    //cout<<tr.number_of_nodes()<<endl;
    //tr.serialize("trie.dump");
    manual_inp();
}
