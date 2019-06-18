def prog0_initial_list():
    send = []
    a = (2**31)-1
    i = 127
    p = 826
    for i in range(127):
        p *= 8505
        p %= a
        p *= 129749
        p += 12345
        p %= a
        send.append(p%10000)
    return send

send = prog0_initial_list()
for s in send:
    print(s)
