from flask import Flask, jsonify
import ctypes

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
            result["registers"].append("0x%08x" % temuHandler.mmu_register_read(i))

        return jsonify(result)
    else:
        return jsonify({
            "valid": False,
            "message": "You need to reset temu."
        })


@app.route('/ddr/write/<addrStr>/<dataStr>')
def temuWriteDDR(addrStr, dataStr):
    if temuHandler:
        pass
    else:
        return jsonify({
            "valid": False,
            "message": "You need to reset temu."
        })


if __name__ == '__main__':
    app.run()
