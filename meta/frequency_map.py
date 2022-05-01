import string
from utils import cpp


def code_gen():
    occurrences = [0] * 256
    with open("frequency_map.txt", "rb") as sample:
        contents = sample.read().rstrip()
        for ch in contents:
            occurrences[ch] += 1
        total = len(contents)
        occurrences = [entry/total for entry in occurrences]

    header_writer = cpp.HeaderWriter(
        name="frequency_table",
        includes=["array"],
    )

    header_writer.write(
        "typedef std::array<double, 256> Frequency;\n"
        "extern constexpr Frequency FREQUENCY_MAP = {\n")
    for i, entry in enumerate(occurrences):
        header_writer.write("    ")
        if chr(i) == '\n':
            header_writer.write("/* \\n */")
        elif chr(i) == '\t':
            header_writer.write("/* \\t */")
        elif chr(i) == '\r':
            header_writer.write("/* \\r */")
        elif chr(i) in string.printable and i > 12:
            header_writer.write(f"/* {chr(i)}  */")
        else:
            header_writer.write("        ")
        header_writer.write(f" {format(entry, '.60g') },\n")
    header_writer.write("};\n\n")

    header_writer.finalize()
