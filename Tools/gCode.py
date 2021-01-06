import os
import shutil

from collections import defaultdict

from cantools import database
from cantools.database.can.c_source import generate
from cantools.database.can.c_source import camel_to_snake_case

from jinja2 import Environment, PackageLoader

CAN_NAME = "M_CAN"
CAN_INSTANCE = 0
CAN_NODE_INDEX = 1
CAN_DBC_PATH = "dbc/20200303_KU_2020_Multi_v20.03.01.dbc"


class Args(object):
    pass


def generate_c_source():
    db = database.load_file(CAN_DBC_PATH)

    database_name = CAN_NAME.lower()

    filename_h = database_name + '.h'
    filename_c = database_name + '.c'

    header, source, *_ = generate(db, database_name, filename_h, filename_c, "no_use_filename_c", bit_fields=True)

    with open(filename_h, 'w') as fd:
        fd.write(header)

    with open(filename_c, 'w') as fd:
        fd.write(source)

    print('Successfully generated {} and {}.'.format(filename_h, filename_c))


def generate_app_code():
    db = database.load_file(CAN_DBC_PATH)
    node_name = db.nodes[CAN_NODE_INDEX].name

    for msg in db.messages:
        msg.snake_name = camel_to_snake_case(msg.name)
        for signal in msg.signals:
            signal.snake_name = camel_to_snake_case(signal.name)

    db.send_msgs = sorted(filter(lambda ms: node_name in ms.senders, db.messages), key=lambda ms: ms.frame_id)

    db.category_send_msgs = defaultdict(list)
    for msg in db.send_msgs:
        if msg.send_type == "Cyclic":
            db.category_send_msgs[msg.cycle_time].append(msg)

    def recv_finder(ms):
        for sig in ms.signals:
            if node_name in sig.receivers:
                return True

        return False

    db.recv_msgs = sorted(filter(recv_finder, db.messages), key=lambda ms: ms.frame_id)

    inst = Args()
    inst.can_name = CAN_NAME
    inst.can_instance = CAN_INSTANCE
    inst.send_msg_nums = len(db.send_msgs)
    inst.recv_msg_nums = len(db.recv_msgs)

    env = Environment(loader=PackageLoader('gCode', 'templates'))

    for temp_name in ["il.h", "il.c", "impl.h", "impl.c"]:
        template = env.get_template(temp_name)
        with open("{}_{}".format(CAN_NAME.lower(), temp_name), "wt") as fd:
            fd.write(template.render(inst=inst, db=db))


def copy_files():
    if not os.path.exists(CAN_NAME.upper()):
        os.mkdir(CAN_NAME.upper())

    for temp_name in [".h", ".c", "_il.h", "_il.c", "_impl.h", "_impl.c"]:
        shutil.copy("{}{}".format(CAN_NAME.lower(), temp_name), CAN_NAME.upper())
        os.remove("{}{}".format(CAN_NAME.lower(), temp_name))


def main():
    generate_c_source()
    generate_app_code()
    copy_files()


if __name__ == '__main__':
    main()
