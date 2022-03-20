using Base64
function hexToBase64(st::AbstractString)
    bytes = hex2bytes(st)
    return base64encode(bytes)
end

test_str = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d"
x = hexToBase64(test_str)
print(x)