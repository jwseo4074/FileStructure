#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#define Max 511

using namespace std;

struct Node {
    float left_down_point_weight;
    // 왼쪽 아래 몸무게 point
    float left_down_point_height;
    // 왼쪽 아래 키 point
    float right_up_point_weight;
    // 오른쪽 위 몸무게 point
    float right_up_point_height;
    // 오른쪽 위 키 point
    bool is_leaf;
    // 리프 노드인지 확인합니다.
    int n;
    // 자료의 개수를 의미합니다.
    float data[2][Max];
    //vector < vector < float > > data(2, vector<float>(Max, 0));
    //vector <float> data[2][Max];
    // 노드에 들어갈 자료의 배열입니다.

    Node *ptr[Max+1];
    // 노드 포인터의 배열.


}*root = NULL, *new_node = NULL, *x = NULL;

Node * init() {
    // 노드를 초기화 함수
    new_node = new Node;
    // 객체를 할당합니다.
    new_node->left_down_point_weight = 0.0;
    new_node->left_down_point_height = 0.0;
    new_node->right_up_point_weight = 0.0;
    new_node->right_up_point_height = 0.0;

    for (int i = 0; i < 2; i++) {
        for (int j=0; j< Max; j++ ){
            new_node->data[i][j] = 0.0;
        }
    }

    for (int i = 0; i < Max+1; i++) {
        new_node->ptr[i] = NULL;
    }
    // new_node->data = {0};
    // 노드 한개가 블럭 1개 => 4096 => 레코드 34개
    // new_node->ptr = {0};
    // M + 1개의 자식 노드.
    new_node->is_leaf = true;
    // 생성된거는 기본적으로 리프 노드.
    new_node->n = 0;
    // 자료의 개수는 0개.

    return new_node;
}

struct Qeury {
    float weight_min;
    float weight_max;
    float height_min;
    float height_max;
};

struct Qeury Qeury_struct[100];

Node *supernode = init();
Node *R_tree = init();


float* Childnode_split(Node *orign_node, int i) {
    // x노드를 분할하여 np1과 np3을 할당합니다.
    float mid_0;
    float mid_1;
    int j;
    Node *np1, *np3, *y;

    /* 분할했을때에 np1 이 부모 노드가 되고(위에),
        왼쪽이 orign_node , 오른쪽이 np3  */

    np3 = init();
    np3->is_leaf = true;

    if (i == -2) {
        mid_0 = orign_node->data[0][Max / 2];
        mid_1 = orign_node->data[1][Max / 2];

        orign_node->data[0][Max / 2] = 0.0;
        orign_node->data[1][Max / 2] = 0.0;
        orign_node->n--;

        np1 = init();
        np1->is_leaf = false;

        np3->is_leaf = false;

        orign_node->is_leaf = false;

        if (orign_node != supernode){

            for ( int k=0; k<supernode->n; k++){
                np1->data[0][k] = supernode->data[0][k];
                np1->data[1][k] = supernode->data[1][k];

                np1->ptr[k] = supernode->ptr[k];
                np1->n++;
            }

            np1->data[0][supernode->n] = mid_0;
            np1->data[1][supernode->n] = mid_1;

            np1->ptr[supernode->n] = orign_node;
            //왼쪽 자식 노드는 orign_node
            np1->ptr[supernode->n + 1] = np3;
            //오른쪽 자식 노드는 orign_node
            np1->n++;
        }
        else {
            np1->data[0][np1->n] = mid_0;
            np1->data[1][np1->n] = mid_1;
            //data 는 float 34개짜리 배열
            np1->ptr[np1->n] = orign_node;
            //왼쪽 자식 노드는 orign_node
            np1->ptr[np1->n + 1] = np3;
            //오른쪽 자식 노드는 orign_node
            np1->n++;
        }

        for(j = Max / 2 + 1; j < Max; j++){
        // np3는 x의 오른쪽 부분 노드를 가져감. 18부터 34까지
            np3->data[0][j - (Max / 2 + 1)] = orign_node->data[0][j];
            np3->data[1][j - (Max / 2 + 1)] = orign_node->data[1][j];
            //np3->ptr[j - (Max / 2 + 1)] = orign_node->ptr[j];
            np3->n++;

            // x는 반 쪼개서 왼쪽 부분 노드만 가져감.
            orign_node->data[0][j] = 0.0;
            orign_node->data[1][j] = 0.0;
            orign_node->n--;
            // 원래 데이터(orign_node)에서 왼쪽만 사용
        }
        for(j = Max / 2 + 1; j < Max+1; j++)
            np3->ptr[j - (Max / 2 + 1)] = orign_node->ptr[j];
        // 루트 노드로 설정해줍니다.
        root = np1;
        supernode = np1;
    }
    // orign_node 의 부모 노드를 만들어준다.
    else if(i == -1) {
        // Max의 중간값(255)을 기준으로 절반으로 쪼갠다.
        mid_0 = orign_node->data[0][Max / 2];
        mid_1 = orign_node->data[1][Max / 2];
        // mid 는 orign_node 의 레코드 개수(17)
        orign_node->data[0][Max / 2] = 0.0;
        orign_node->data[1][Max / 2] = 0.0;

        // 17개 레코드 0으로 초기화
        orign_node->n--;

        np1 = init();

        // np1는 부모노드임. 리프 노드 x.
        // orign_node 는 왼쪽으로 빠짐 , 리프 노드
        np1->is_leaf = false;
        orign_node->is_leaf = true;

        for(j = Max / 2 + 1; j < Max; j++){
        // np3는 x의 오른쪽 부분 노드를 가져감. 18부터 34까지
            np3->data[0][j - (Max / 2 + 1)] = orign_node->data[0][j];
            np3->data[1][j - (Max / 2 + 1)] = orign_node->data[1][j];

            np3->ptr[j - (Max / 2 + 1)] = orign_node->ptr[j];

            np3->n++;

            // x는 반 쪼개서 왼쪽 부분 노드만 가져감.
            orign_node->data[0][j] = 0.0;
            orign_node->data[1][j] = 0.0;

            orign_node->n--;
            // 원래 데이터(orign_node)에서 왼쪽만 사용
        }

        // x의 모든 자식 노드를 NULL로 만듭니다.
        for(j = 0; j < Max + 1; j++) {
            orign_node->ptr[j] = NULL;
        } // x 는 리프 노드 니깐

        np1->data[0][0] = mid_0;
        np1->data[1][0] = mid_1;

        //data 는 float 34개짜리 배열
        np1->ptr[np1->n] = orign_node;
        //왼쪽 자식 노드는 orign_node
        np1->ptr[np1->n + 1] = np3;
        //오른쪽 자식 노드는 orign_node
        np1->n++;

        // 루트 노드로 설정해줍니다.
        root = np1;
        supernode = np1;
    }

    // 이미 부모 노드가 있을때.
    else    {
        y = orign_node->ptr[i];
        // y는 orign_node 의 i 번째 자식 노드
        mid_0 = y->data[0][Max / 2];
        mid_1 = y->data[1][Max / 2];
        y->data[0][Max / 2] = 0.0;
        y->data[1][Max / 2] = 0.0;
        y->n--;
        for(j = Max / 2 + 1; j < Max; j++)  {

            // np3은 x의 오른쪽 자식 부분만 가져갑니다.
            np3->data[0][j - (Max / 2 + 1)] = y->data[0][j];
            np3->data[1][j - (Max / 2 + 1)] = y->data[1][j];
            np3->n++;
            y->data[0][j] = 0.0;
            y->data[1][j] = 0.0;
            y->n--;
        }
        for ( int k=Max-1; k>i ; k--){
            orign_node->ptr[k+1] = orign_node->ptr[k];
        }
        orign_node->ptr[i] = y;
        orign_node->ptr[i + 1] = np3;
    }
    float *floatarr = new float[2];
    floatarr[0] = mid_0;
    floatarr[1] = mid_1;

    return floatarr;
}

void sort(float *swap, int n) {
    int i,j;
    float temp = 0.0;
    for(i = 0; i < n; i++) {
        for(j = i; j <= n; j++)   {
            if(swap[i] > swap[j]) {
                temp = swap[i];
                swap[i] = swap[j];
                swap[j] = temp;
            }
        }
    }
}

void display(Node* node){
   if(node!=NULL)   {
      for(int i = 0; i < node->n; i++)
         cout<<"(" << node->data[0][i]<<"," << node->data[1][i] << ") ";
      cout<<"\n";
      if(node->is_leaf != true) {
         for(int i = 0; i < node->n+1; i++)
            display(node->ptr[i]);
      }
   }
}

int k = 0;

void insert(float weight, float height) {
    int i;
    float temp_0;
    float temp_1;
    x = root;
    supernode = root;
    if(x == NULL){
    // 루트 노드가 NULL이라면 (비어있다면)
        root = init();
        x = root;
        supernode = root;
    }
    else{
        if(x->is_leaf == true && x->n == Max) {
        // 현재 리프노드이고 크기가 꽉 찬 경우 (노드에 데이터(레코드)가 511개(최대)인 경우 )
            Childnode_split(x, -1);
            //노드 분해
            x = root;
            supernode = root;

            for(i = 0; i < (x->n); i++) {
            // 값을 이동하며 삽입될 자리를 찾습니다.
                if( (weight > x->data[0][i]) && (weight < x->data[0][i + 1]) ) {
                    i++;
                    break;
                }
                else if(weight < x->data[0][0]) {
                    break;
                }
                else {
                    continue;
                }
            }
            x = x->ptr[i];
        }
        else {
            // 리프노드가 아닐 때입니다.
            while(x->is_leaf == false) {
            // 리프 노드까지 이동합니다.
                if (x->n == Max ){
                    float *floatarr1 = Childnode_split(x, -2);
                    temp_0 = floatarr1[0];
                    temp_1 = floatarr1[1];
                    delete[] floatarr1;
                    x->data[0][x->n] = temp_0;
                    x->data[1][x->n] = temp_1;
                    x=root;
                    supernode = root;
                }
                for(i = 0; i < (x->n); i++) {
                    if((weight > x->data[0][i]) && (weight < x->data[0][i + 1])) {
                        i++;
                        break;
                    }
                    else if(weight < x->data[0][0]) {
                        break;
                    }
                    else {
                        continue;
                    }
                }
                if((x->ptr[i])->n == Max){
                // 리프 노드 위에서도 만약에 최대치를 넘으면 분할해줍니다.
                    float *floatarr2 = Childnode_split(x, i);

                    temp_0 = floatarr2[0];
                    temp_1 = floatarr2[1];
                    delete[] floatarr2;

                    x->data[0][x->n] = temp_0;
                    x->data[1][x->n] = temp_1;
                    x->n++;
                    // if (supernode->n == Max){
                    //     cout << " \n\n super node is full \n\n" << endl;
                    //     temp = Childnode_split(x, -2);
                    //     x->data[x->n] = temp;
                    //     sort(x->data,x->n);
                    //     x=root;
                    //     supernode = root;
                    // }
                    //continue;
                }
                else {
                    x = x->ptr[i];
                }
            }
        }
    }
    x->data[0][x->n] = weight;
    x->data[1][x->n] = height;
    x->n++;
}


void make_Rtree(vector <float> &weight_v, vector <float> &height_v ){
    root = R_tree;
    for (int i=0; i< weight_v.size(); i++){
        insert(weight_v[i], height_v[i]);
    }
    R_tree = root;
}
int result_count ;

int Rtree_search(Node * selectnode, float weight_start, float weight_end, float height_start, float height_end ){
    float weight_min = weight_start;
    float weight_max = weight_end;
    float height_min = height_start;
    float height_max = height_end;

    if(selectnode!=NULL)   {
        for(int i = 0; i < selectnode->n; i++){
            if (weight_min <= selectnode->data[0][i] && selectnode->data[0][i] <= weight_max){
                if (height_min <= selectnode->data[1][i] && selectnode->data[1][i] <= height_max)
                    result_count++;
            }
        }
        if(selectnode->is_leaf != true) {
            for(int i = 0; i < selectnode->n+1; i++)
                Rtree_search(selectnode->ptr[i], weight_min, weight_max,height_min, height_max);
        }
    }
}
int size;

int main() {

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    cout << " << 10K Start >> \n " << endl;
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    ifstream fin10K("student10K.inp");
    //int size;
    string str;
    fin10K >> size;
    float num_float;
    int num_int;
    vector <string> result;
    vector <float> weight_vec;
    vector <float> height_vec;

    while (!fin10K.eof()) {
        getline(fin10K, str);
        //cout << str << endl;
        string token;
        stringstream ss(str);
        int k = 0;
        string value;
        while (getline(ss, token, ',')){
            result.push_back(token);
            value = *(result.end() - 1);
            //cout <<value <<endl;
            if (k == 3) {
                num_float = stof(value);
                weight_vec.push_back(num_float);
            }
            if (k == 4) {
                num_float = stof(value);
                height_vec.push_back(num_float);
            }
            k++;
        }
        result.clear();
    }
    fin10K.close();

    make_Rtree(weight_vec,height_vec);

    ifstream fin_qry("students.qry");
    int size_qry;
    string str_qry;
    fin_qry >> size_qry;

    vector <string> result_qry;
    int index_qry = 0;

    while (!fin_qry.eof()) {
        getline(fin_qry, str_qry);
        str_qry.replace(0,3,"");
        string token_qry;
        stringstream ss_qry(str_qry);
        int k = 0;
        string value_qry;

        while (getline(ss_qry, token_qry, ',')){
            result_qry.push_back(token_qry);
            value_qry = *(result_qry.end() - 1);
            if (k == 0) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_min = num_float;
            }
            if (k == 1) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_max = num_float;
            }
            if (k == 2) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_min = num_float;
            }
            if (k == 3) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_max = num_float;
            }
            k++;
        }
        index_qry++;
    }
    fin_qry.close();

    vector <int> result_answer;

    for(int i=1; i<=size_qry; i++){
        result_count = 0;
        Rtree_search(R_tree, Qeury_struct[i].weight_min, Qeury_struct[i].weight_max, Qeury_struct[i].height_min, Qeury_struct[i].height_max);
        result_answer.push_back(result_count);
        result_count = 0;
    }

    ofstream fout_out10K("r10K.out");
    for(int i=0; i<result_answer.size(); i++) {
        fout_out10K << result_answer[i] << "\n";
    }
    result_answer.clear();
    fout_out10K.close();

    x = init();
    root = init();
    R_tree = init();

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    cout << " << 20K Start >> \n " << endl;
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    ifstream fin20K("student20K.inp");
    //int size;
    str = "";
    fin20K >> size;
    num_float = 0.0;
    num_int = 0.0;
    result.clear();
    weight_vec.clear();
    height_vec.clear();

    while (!fin20K.eof()) {
        getline(fin20K, str);
        string token;
        stringstream ss(str);
        int k = 0;
        string value;
        while (getline(ss, token, ',')){
            result.push_back(token);
            value = *(result.end() - 1);
            if (k == 3) {
                num_float = stof(value);
                weight_vec.push_back(num_float);
            }
            if (k == 4) {
                num_float = stof(value);
                height_vec.push_back(num_float);
            }
            k++;
        }
        result.clear();
    }
    fin20K.close();

    make_Rtree(weight_vec,height_vec);

    ifstream fin_qry_20K("students.qry");
    size_qry = 0;
    str_qry = "";
    fin_qry_20K >> size_qry;

    result_qry.clear();
    index_qry = 0;

    while (!fin_qry_20K.eof()) {
        getline(fin_qry_20K, str_qry);
        str_qry.replace(0,3,"");
        string token_qry;
        stringstream ss_qry(str_qry);
        int k = 0;
        string value_qry;

        while (getline(ss_qry, token_qry, ',')){
            result_qry.push_back(token_qry);
            value_qry = *(result_qry.end() - 1);
            if (k == 0) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_min = num_float;
            }
            if (k == 1) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_max = num_float;
            }
            if (k == 2) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_min = num_float;
            }
            if (k == 3) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_max = num_float;
            }
            k++;
        }
        index_qry++;
    }
    fin_qry_20K.close();

    result_answer.clear();

    for(int i=1; i<=size_qry; i++){
        result_count = 0;
        Rtree_search(R_tree, Qeury_struct[i].weight_min, Qeury_struct[i].weight_max, Qeury_struct[i].height_min, Qeury_struct[i].height_max);
        result_answer.push_back(result_count);
        result_count = 0;
    }

    ofstream fout_out20K("r20K.out");
    for(int i=0; i<result_answer.size(); i++) {
        fout_out20K << result_answer[i] << "\n";
    }
    result_answer.clear();
    fout_out20K.close();

    x = init();
    root = init();
    R_tree = init();

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    cout << " << 30K Start >> \n " << endl;
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    ifstream fin30K("student30K.inp");
    //int size;
    str = "";
    fin30K >> size;
    num_float = 0.0;
    num_int = 0.0;
    result.clear();
    weight_vec.clear();
    height_vec.clear();

    while (!fin30K.eof()) {
        getline(fin30K, str);
        string token;
        stringstream ss(str);
        int k = 0;
        string value;
        while (getline(ss, token, ',')){
            result.push_back(token);
            value = *(result.end() - 1);
            if (k == 3) {
                num_float = stof(value);
                weight_vec.push_back(num_float);
            }
            if (k == 4) {
                num_float = stof(value);
                height_vec.push_back(num_float);
            }
            k++;
        }
        result.clear();
    }
    fin30K.close();

    make_Rtree(weight_vec,height_vec);

    ifstream fin_qry_30K("students.qry");
    size_qry = 0;
    str_qry = "";
    fin_qry_30K >> size_qry;

    result_qry.clear();
    index_qry = 0;

    while (!fin_qry_30K.eof()) {
        getline(fin_qry_30K, str_qry);
        str_qry.replace(0,3,"");
        string token_qry;
        stringstream ss_qry(str_qry);
        int k = 0;
        string value_qry;

        while (getline(ss_qry, token_qry, ',')){
            result_qry.push_back(token_qry);
            value_qry = *(result_qry.end() - 1);
            if (k == 0) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_min = num_float;
            }
            if (k == 1) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_max = num_float;
            }
            if (k == 2) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_min = num_float;
            }
            if (k == 3) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_max = num_float;
            }
            k++;
        }
        index_qry++;
    }
    fin_qry_30K.close();

    result_answer.clear();

    for(int i=1; i<=size_qry; i++){
        result_count = 0;
        Rtree_search(R_tree, Qeury_struct[i].weight_min, Qeury_struct[i].weight_max, Qeury_struct[i].height_min, Qeury_struct[i].height_max);
        result_answer.push_back(result_count);
        result_count = 0;
    }

    ofstream fout_out30K("r30K.out");
    for(int i=0; i<result_answer.size(); i++) {
        fout_out30K << result_answer[i] << "\n";
    }
    result_answer.clear();
    fout_out30K.close();

    x = init();
    root = init();
    R_tree = init();

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    cout << " << 40K Start >> \n " << endl;
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    ifstream fin40K("student40K.inp");
    //int size;
    str = "";
    fin40K >> size;
    num_float = 0.0;
    num_int = 0.0;
    result.clear();
    weight_vec.clear();
    height_vec.clear();

    while (!fin40K.eof()) {
        getline(fin40K, str);
        string token;
        stringstream ss(str);
        int k = 0;
        string value;
        while (getline(ss, token, ',')){
            result.push_back(token);
            value = *(result.end() - 1);
            if (k == 3) {
                num_float = stof(value);
                weight_vec.push_back(num_float);
            }
            if (k == 4) {
                num_float = stof(value);
                height_vec.push_back(num_float);
            }
            k++;
        }
        result.clear();
    }
    fin40K.close();

    make_Rtree(weight_vec,height_vec);

    ifstream fin_qry_40K("students.qry");
    size_qry = 0;
    str_qry = "";
    fin_qry_40K >> size_qry;

    result_qry.clear();
    index_qry = 0;

    while (!fin_qry_40K.eof()) {
        getline(fin_qry_40K, str_qry);
        str_qry.replace(0,3,"");
        string token_qry;
        stringstream ss_qry(str_qry);
        int k = 0;
        string value_qry;

        while (getline(ss_qry, token_qry, ',')){
            result_qry.push_back(token_qry);
            value_qry = *(result_qry.end() - 1);
            if (k == 0) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_min = num_float;
            }
            if (k == 1) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_max = num_float;
            }
            if (k == 2) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_min = num_float;
            }
            if (k == 3) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_max = num_float;
            }
            k++;
        }
        index_qry++;
    }
    fin_qry_40K.close();

    result_answer.clear();

    for(int i=1; i<=size_qry; i++){
        result_count = 0;
        Rtree_search(R_tree, Qeury_struct[i].weight_min, Qeury_struct[i].weight_max, Qeury_struct[i].height_min, Qeury_struct[i].height_max);
        result_answer.push_back(result_count);
        result_count = 0;
    }

    ofstream fout_out40K("r40K.out");
    for(int i=0; i<result_answer.size(); i++) {
        fout_out40K << result_answer[i] << "\n";
    }
    result_answer.clear();
    fout_out40K.close();

    return 0;
}
