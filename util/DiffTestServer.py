from flask import Flask, jsonify, request
import ctypes
import json

DIFFTEST_PATH = "../cmake-build-debug/libtemudifftest.dylib"
temuHandler = None

app = Flask(__name__)


@app.route('/')
def hello():
    return "TEMU DiffTest Server"


@app.route('/reset')
def temuReset():
    global temuHandler
    if temuHandler is None:
        temuHandler = ctypes.CDLL(DIFFTEST_PATH)
    temuHandler.lib_set_program_counter(ctypes.c_uint32(0x80000000))
    return jsonify({
        "valid": True
    })


@app.route('/exec/<instStr>')
def temuExecInstruction(instStr):
    if temuHandler:
        inst = ctypes.c_uint32(eval("0x" + instStr))
        temuHandler.decode(inst)
        result = {
            "valid": True,
            "registers": [],
            "pc": "0x%08x" % (temuHandler.lib_get_program_counter() & 0xffffffff)
        }
        for i in range(0, 32):
            result["registers"].append("0x%08x" % (temuHandler.mmu_register_read(i) & 0xffffffff))

        return jsonify(result)
    else:
        return jsonify({
            "valid": False,
            "message": "You need to reset temu."
        })


@app.route("/exec/batch/<compareWhat>", methods=['POST'])
def temuExecInstBatch(compareWhat):
    data = request.get_data()
    data = json.loads(data)
    if temuHandler:
        if compareWhat == "pcAndFinalReg":
            result = {
                "valid": True,
                "registers": [],
                "pc_history": []
            }
            for instStr in data["instructions"]:
                inst = ctypes.c_uint32(eval("0x" + instStr))
                temuHandler.decode(inst)
                result["pc_history"].append("0x%08x" % (temuHandler.lib_get_program_counter() & 0xffffffff))

            for i in range(0, 32):
                result["registers"].append("0x%08x" % (temuHandler.mmu_register_read(i) & 0xffffffff))
            return jsonify(result)
        elif compareWhat == "pcAndEachReg":
            result = {
                "valid": True,
                "registers": [],
                "pc_history": []
            }
            for instStr in data["instructions"]:
                inst = ctypes.c_uint32(eval("0x" + instStr))
                temuHandler.decode(inst)
                result["pc_history"].append("0x%08x" % (temuHandler.lib_get_program_counter() & 0xffffffff))
                reg_hist = []
                for i in range(0, 32):
                    reg_hist.append("0x%08x" % (temuHandler.mmu_register_read(i) & 0xffffffff))

                result["registers"].append(reg_hist)

            return jsonify(result)
        else:
            return jsonify({
                "valid": False,
                "message": "Not supported compare method settings."
            })
    else:
        return jsonify({
            "valid": False,
            "message": "You need to reset temu."
        })


@app.route('/ddr/write/<addrStr>/<dataStr>')
def temuWriteDDR(addrStr, dataStr):
    if temuHandler:
        addr = ctypes.c_uint32(eval("0x" + addrStr))
        data = ctypes.c_uint32(eval("0x" + dataStr))
        write_res = temuHandler.lib_memory_write_w(addr, data)
        if write_res:
            return jsonify({
                "valid": False,
                "message": "Address bad or not aligned."
            })
        else:
            return jsonify({
                "valid": True,
                "message": "Data is written to sim ddr."
            })
    else:
        return jsonify({
            "valid": False,
            "message": "You need to reset temu."
        })


if __name__ == '__main__':
    app.run()
