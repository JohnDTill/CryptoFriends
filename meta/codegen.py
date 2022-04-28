import frequency_map
import os


def should_run():
    if not os.path.isdir("../src/generated"):
        return True
    included_extensions = ['csv', 'py']
    src = [fn for fn in os.listdir(".") if any(fn.endswith(ext) for ext in included_extensions)]
    latest_change = max([os.path.getmtime("./" + file) for file in src])
    file_change_times = [os.path.getmtime("../src/generated/" + file) for file in os.listdir("../src/generated")]
    if not file_change_times:
        return True
    latest_run = min(file_change_times)
    print(f"Codegen: latest change @ {latest_change}, latest run @ {latest_run}")
    return latest_change > latest_run


def main():
    assert os.path.basename(os.getcwd()) == "meta", "Must run codegen from meta directory"
    if not should_run():
        return
    os.makedirs("../src/generated", exist_ok=True)

    frequency_map.code_gen()


if __name__ == "__main__":
    main()
