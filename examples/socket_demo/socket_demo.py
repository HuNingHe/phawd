# robot controller
from phawd import SocketToPhawd, SocketFromPhawd, SocketConnect, Parameter

if __name__ == '__main__':
    p0 = Parameter("pw_d", 1.213)
    p1 = Parameter("pw_s64", 12)
    p2 = Parameter("pw_vec3d", [-1.4, 2, 3])

    send_params_num = 3
    read_params_num = 3
    send_size = SocketToPhawd.__sizeof__() + send_params_num * Parameter.__sizeof__()
    read_size = SocketFromPhawd.__sizeof__() + read_params_num * Parameter.__sizeof__()

    client = SocketConnect()

    client.init(send_size, read_size)
    client.connectToServer("127.0.0.1", 5230)
    run_iter = 0

    socket_to_phawd = client.getSend()
    socket_to_phawd.numWaveParams = 3
    socket_to_phawd.parameters = [p0, p1, p2]

    while run_iter < 5000000:
        run_iter += 1
        socket_to_phawd = client.getSend()

        ret = client.read()

        if ret > 0:
            socket_from_phawd = client.getRead()

            p0.setValue(socket_from_phawd.parameters[0].getDouble())
            p1.setValue(socket_from_phawd.parameters[1].getS64())
            p2.setValue(socket_from_phawd.parameters[2].getVec3d())
            socket_to_phawd.parameters = [p0, p1, p2]
            client.send()

            if run_iter % 20 == 0:
                print(socket_from_phawd.parameters[0].getName(), ":")
                print(socket_from_phawd.parameters[0].getDouble())
                print(socket_from_phawd.parameters[1].getName(), ":")
                print(socket_from_phawd.parameters[1].getS64())
                print(socket_from_phawd.parameters[2].getName(), ":")
                print(socket_from_phawd.parameters[2].getVec3d())