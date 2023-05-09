# robot controller
from phawd import SharedMemory, SharedParameters, ParameterCollection, Parameter

if __name__ == '__main__':
    p0 = Parameter("pw_d", 12.13)
    p1 = Parameter("pw_s64", 1213)
    p2 = Parameter("pw_vec3d", [-1.4, 2, 3])

    shm = SharedMemory()
    pc = ParameterCollection()

    p_size = Parameter.__sizeof__()
    sp_size = SharedParameters.__sizeof__()
    num_control_params = 3
    num_wave_params = 3
    shm_size = sp_size + (num_control_params + num_wave_params) * p_size

    shm.attach("demo", shm_size)
    sp = shm.get()
    sp.setParameters([p0, p1, p2])
    sp.connected += 1
    ctr_params = sp.getParameters()
    sp.collectParameters(pc)

    run_iter = 0

    while run_iter < 500000:
        run_iter += 1
        p0.setValue(pc.lookup("pd").getDouble())
        p1.setValue(pc.lookup("ps64").getS64())
        p2.setValue(pc.lookup("pvec3d").getVec3d())
        sp.setParameters([p0, p1, p2])

        if run_iter % 20 == 0:
            print(pc.lookup("ps64").getName(), ":")
            print(pc.lookup("ps64").getS64())
            print(pc.lookup("pd").getName(), ":")
            print(pc.lookup("pd").getDouble())
            print(pc.lookup("pvec3d").getName(), ":")
            print(pc.lookup("pvec3d").getVec3d())

    sp.connected -= 1