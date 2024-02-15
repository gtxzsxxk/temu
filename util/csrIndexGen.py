# 读取zicsr.h中的csr寄存器定义，把它们排好序，
# 并且弄上顺序，方便数组索引计算

class CsrInformation:
    def __init__(self, lines: list):
        self.csrName = lines[0].split(' ')[1].split('_')[2]
        for defineLine in lines:
            if defineLine.__contains__("_num_"):
                self.csrNumber = defineLine.split(' ')[2]
            elif defineLine.__contains__("_prv_"):
                self.csrPrivilege = defineLine.replace("#define CSR_prv_" + self.csrName + " ", "")


zicsrSource = '../include/zicsr.h'
matchBegin = '/* === csrIndexGen match begin DO NOT MODIFY THIS LINE === */'
matchEnd = '/* === csrIndexGen match end DO NOT MODIFY THIS LINE === */'

fp = open(zicsrSource, "r")
codeLines = fp.readlines()
fp.close()

startIndex = 0
endIndex = 0


def readCsrDefinitions():


for i in range(0, codeLines.__len__()):
    if codeLines[i] == '':
        continue
    if i == matchBegin:
        startIndex = i
