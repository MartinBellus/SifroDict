#include<bits/stdc++.h>
using namespace std;

class Node{
    public:
        map<char,Node*> children;
        Node* parent;
        char c;
        int freq = 0;
        Node(char ch,Node* p = nullptr,int f = 0){
            parent = p;
            c = ch;
            freq = f;
        }
        Node* add_child(char ch){
            if(children.find(ch) == children.end()) children[ch] = new Node(ch,this);
            return children[ch];
        }
};

class Trie{
    public:
        Node* root;
        Trie(){
            root = new Node(':');
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
        void match_pattern(string pattern,vector<pair<string,int>>* out,int ind = 0,Node* cur = nullptr){
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
                out->push_back({ans,f});
                return;
            }
            if(ind == pattern.size()) return; //no match
            if(pattern[ind] == '.'){ //resolve wildcard character
                for(auto it = cur->children.begin();it != cur->children.end();it++){
                    match_pattern(pattern,out,ind + 1, it->second);
                }
            }
            if(cur->children.find(pattern[ind]) != cur->children.end()){
                match_pattern(pattern,out,ind + 1,cur->children[pattern[ind]]);    
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
};

Trie tr = Trie();

bool comp(pair<string,int> a,pair<string,int> b){
    return a.second > b.second || (a.second == b.second && a.first > b.first);
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
            vector<pair<string,int>> ans;
            co += '$';
            tr.match_pattern(co,&ans);
            sort(ans.begin(),ans.end(),comp);
            for(auto i : ans) cout<<i.first<<" "<<i.second<<endl;
        }else if(id == 'd') tr.print_words();
        else break;
    };
}

void load_wordlist(string file){
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
}

int main(){
    load_wordlist("sk_parsed.txt");
    manual_inp();
}
