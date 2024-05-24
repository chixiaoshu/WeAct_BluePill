from PIL import Image
import sys
import os

# Convert an image to a C-style 2D 16-bit color value array.
# Supports 24-bit and 32-bit images, automatically converting to RGB565 format.
def convert_image_to_array(input_file, output_file):
    img = Image.open(input_file)
    if img.width != 128 or img.height != 128:
        print("Error: 128x128 image expected")
        sys.exit(2)

    # Ensure the image is in RGB mode
    if img.mode != "RGB":
        img = img.convert("RGB")

    output_extension = os.path.splitext(output_file)[1].lower()
    input_name = os.path.splitext(os.path.basename(input_file))[0]
    if output_extension == ".h":
        write_to_header_file(img, input_name, output_file)
    else:
        write_to_text_file(img, input_name, output_file)

    print("Output written to: {}".format(output_file))

# Write the image data to a C header file.
def write_to_header_file(img, input_name, output_file):
    with open(output_file, "w") as f:
        f.write("#ifndef __{}_H__\n".format(os.path.splitext(os.path.basename(output_file))[0].upper()))
        f.write("#define __{}_H__\n\n".format(os.path.splitext(os.path.basename(output_file))[0].upper()))
        f.write("const uint16_t {}[][128] = {{\n".format(input_name))

        for y in range(img.height):
            f.write("    {")
            for x in range(img.width):
                (r, g, b) = img.getpixel((x, y))
                color565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
                color565 = ((color565 & 0xFF00) >> 8) | ((color565 & 0xFF) << 8)
                f.write("0x{:04X},".format(color565))
            f.write("},\n")

        f.write("};\n\n#endif")

# Write the image data to a text file.
def write_to_text_file(img, input_name, output_file):
    with open(output_file, "w") as f:
        f.write("const uint16_t {}[][128] = {{\n".format(input_name))

        for y in range(img.height):
            f.write("    {")
            for x in range(img.width):
                (r, g, b) = img.getpixel((x, y))
                color565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
                color565 = ((color565 & 0xFF00) >> 8) | ((color565 & 0xFF) << 8)
                f.write("0x{:04X},".format(color565))
            f.write("},\n")

        f.write("};")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <input.jpg/.png> [<output.txt/h>]".format(sys.argv[0]))
        sys.exit(1)

    input_file = sys.argv[1]

    if len(sys.argv) > 2:
        output_file = sys.argv[2]
    else:
        base_name = os.path.splitext(os.path.basename(input_file))[0]
        output_file = "{}.h".format(base_name)

    convert_image_to_array(input_file, output_file)
