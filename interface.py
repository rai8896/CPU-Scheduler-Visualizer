import tkinter as tk
from tkinter import filedialog, messagebox
import subprocess

def run_scheduler():
    try:
        # Call the compiled C++ executable
        subprocess.run(["scheduler.exe"], check=True)

        # Read the output.txt file and show in output_text
        with open("output.txt", "r") as f:
            result = f.read()
            output_text.delete("1.0", tk.END)
            output_text.insert(tk.END, result)

    except Exception as e:
        messagebox.showerror("Error", f"Failed to run scheduler: {e}")

def load_input():
    file_path = filedialog.askopenfilename(filetypes=[("Text Files", "*.txt")])
    if file_path:
        with open(file_path, "r") as f:
            input_data = f.read()
            input_text.delete("1.0", tk.END)
            input_text.insert(tk.END, input_data)
        with open("input.txt", "w") as f:
            f.write(input_data)

def save_input():
    with open("input.txt", "w") as f:
        f.write(input_text.get("1.0", tk.END).strip())

root = tk.Tk()
root.title("CPU Scheduler Visualizer")
root.geometry("800x600")

tk.Label(root, text="Process Input (Format: ID ArrivalTime BurstTime Priority):").pack()
input_text = tk.Text(root, height=10)
input_text.pack(fill=tk.X, padx=10)

btn_frame = tk.Frame(root)
btn_frame.pack(pady=5)
tk.Button(btn_frame, text="Load Input", command=load_input).grid(row=0, column=0, padx=5)
tk.Button(btn_frame, text="Save Input", command=save_input).grid(row=0, column=1, padx=5)
tk.Button(btn_frame, text="Run Scheduler", command=run_scheduler).grid(row=0, column=2, padx=5)

tk.Label(root, text="Output:").pack()
output_text = tk.Text(root, height=15)
output_text.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

root.mainloop()
