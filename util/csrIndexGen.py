# 读取zicsr.h中的csr寄存器定义，把它们排好序，
# 并且弄上顺序，方便数组索引计算

class CsrInformation:
    def __init__(self, lines: list):
        self.csrName = lines[0].split(' ')[1].split('_')[2].replace('\r\n', '').replace('\n', '')
        for defineLine in lines:
            if defineLine.__contains__("_num_"):
                self.csrNumber = defineLine.split(' ')[2].replace('\r\n', '').replace('\n', '')
            elif defineLine.__contains__("_prv_"):
                self.csrPrivilege = (defineLine.replace("#define CSR_prv_" + self.csrName + " ", "")
                                     .replace('\r\n', '').replace('\n', ''))

    def __str__(self):
        return "CSR NAME=%s\n    NUMBER=%s\n    PRIV=%s\n\n" % (self.csrName, self.csrNumber, self.csrPrivilege)


zicsrSource = '../include/zicsr.h'
matchBegin = '/* === csrIndexGen match begin DO NOT MODIFY THIS LINE === */'
matchEnd = '/* === csrIndexGen match end DO NOT MODIFY THIS LINE === */'

fp = open(zicsrSource, "r")
codeLines = fp.readlines()
fp.close()

startIndex = 0
endIndex = 0
csrList = []


def read_csr_definitions():
    global csrList, startIndex
    while not codeLines[startIndex + 1].__contains__(matchEnd):
        def_index = startIndex + 1
        def_end_index = def_index + 1
        def_lines = [codeLines[def_index]]
        while codeLines[def_end_index] != '\r\n' and codeLines[def_end_index] != '\n':
            if codeLines[def_end_index].__contains__(matchEnd):
                csrList.append(CsrInformation(def_lines))
                return
            def_lines.append(codeLines[def_end_index])
            def_end_index += 1
        startIndex = def_end_index
        csrList.append(CsrInformation(def_lines))


for i in range(0, codeLines.__len__()):
    if codeLines[i] == '':
        continue
    if codeLines[i].__contains__(matchBegin):
        startIndex = i
        read_csr_definitions()
        break

print(csrList)
