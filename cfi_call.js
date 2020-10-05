/* TO DO : 선언하지않은 callindirect 들이 table에서 발견될 경우 예외처리? 하지만 필요없음,
 어차피 소팅해서 순서대로 table에 넣을거면 그 이후의 index에 선언하지 않은 함수들이 들어갈 것이기 때문
그러므로 이제 빌트인 타입은 TYPEVAL 객체에 넣어놓고 유저디파인드는 하나씩 추가해주는 셈이고
wat에서만 잘 처리해주면 된다 . -s EXPORT_FUNCTIONS 옵션으로 wasm 만들 때 그냥 export 해버리고,
js 파일에서 걔네를 wrapping 하는 부분을 없앤다. 그리고, Classifier에서 인덱싱을 해주고 wat파일에서
elem에서 제거하고 호출부를 함수명으로 쉽게 찾을 수 있으므로, index를 stack에 push 하는부분만 수정해준다.
이후에 돌아가는지 확인이 되었으면, 예시코드를 얼렁 작성하고, wat에다가 인덱스범위체크하는부분을 추가한다. 
 */
const fs = require('fs');
const TYPEVAL = {
    'nil' : -1,
    'void' : 0,
    'bool' : 1,
    'char' : 2,
    'unsigned char' : 3,
    'char *' : 4, 
    'short' : 5,
    'unsigned short' : 6,
    'int' : 7,
    'unsigned int' : 8,
    'int *' : 9,
    'long' : 10,
    'unsigned long' : 11,
    'long long' : 12,
    'unsigned long long': 13,
    'float' : 14,
    'double' : 15,
    'long double' : 16,
    'void *' : 17,
    'char *' : 18,
    'int *' : 19,
}
class Classifier {
    constructor(functionSigFile){
        try {
            const data = fs.readFileSync(functionSigFile, 'utf-8');
            const funcDeclArr = data.split('\n');
            this.functionSigList = funcDeclArr.map((decl) => this.#parseSignatureStrings(decl));
            this.originalIdx = this.functionSigList.map((func, idx) => { return { name : func['name'], idx}})
            this.#sortSignatures();
            this.typeCount = Object.keys(TYPEVAL).length + 2;
        } catch(err) {
            console.log("Function Sig File Read Failed", err);
            return;
        }
    }   
    #parseSignatureStrings =  (declStr) => {
        let tokenArr = declStr.split(", ");
        const arrLen = tokenArr.length;
        if(arrLen === 0) throw new Error("Wrong Format, At Least 1 Tokens(Name)");
        if(arrLen < 3) {
            for(let i = 0; i < 3-arrLen; i++) 
                tokenArr.push('nil');
        }
        return {
            name : tokenArr[0],
            ret : this.#getTypeValue(tokenArr[1]),
            params :  [ ...(tokenArr.slice(2).map(
                                (str) => this.#getTypeValue(str)
                              )
                            )
                      ]
        }        
    }
    #getTypeValue = (typeStr) => {
        let expected = TYPEVAL[typeStr];
        if(expected === 'undefined') {
            TYPEVAL[typeStr] = this.typeCount;
            expected = this.typeCount;
            this.typeCount++;
        }
        return expected;
    }
    #sortSignatures = () => {
        this.functionSigList.sort((a, b) => {
            return (a.ret - b.ret) 
                ?  (a.ret - b.ret)
                : this.#compareParams(a.params, b.params)
        })
    }
    #compareParams = (a, b) => {
        const alen = a.length;
        const blen = b.length;
        const gap = Math.abs(alen - blen);
        let swapOrNot = false;
        if(alen > blen){
            for(let i = 0; i < gap; i++)
                b.push(0);
        } else {
            for(let i = 0; i < gap; i++)
                a.push(0);
        }
        for(let i = 0; i< a.length; i++){
            if(a[i] != b[i]){
                swapOrNot = a[i] - b[i];
                break;
            }
        }
        return swapOrNot;
    } 
    getOriginalIdx = () => console.log(this.originalIdx);
    getFunctionSigList = () => console.log(this.functionSigList)
    setExportsIndices = (table, exports) => {
        this.functionSigList.forEach( (funcSig, index) => {
            const funcName = funcSig.name;
            table.set(index+1, exports[funcName]);
        });
    }
}
module.exports = Classifier;