from cs50 import get_int

while True:
    # Get user's input
    height = get_int('Height: ')

    # Pyramid height has to be positive and no greater than 8
    if 0 < height <= 8:
        # Print pyramid
        for i in range(1, height + 1):
            spaces = " " * (height - i)
            blocks = "#" * i
            print(f"{spaces}{blocks}  {blocks}")

        # Break out of the infinite loop
        break