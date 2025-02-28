#!/usr/bin/env python3
import zipfile
import os
import builtins

def print(*args, **kwargs):
    kwargs.setdefault("flush", True)
    builtins.print(*args, **kwargs)

def extract_zip(zip_path):
    """
    Extracts the contents of a ZIP file and returns the name of the folder where it was extracted.
    
    Parameters:
    zip_path (str): Path to the ZIP file.
    
    Returns:
    str: The folder name where the contents were extracted.
    """
    if not os.path.exists(zip_path):
        print(f"Error: The file {zip_path} does not exist.")
        return None

    folder_name = os.path.splitext(os.path.basename(zip_path))[0]
    extract_to = os.path.join(os.path.dirname(zip_path), folder_name)

    if not os.path.exists(extract_to):
        os.makedirs(extract_to)

    try:
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            zip_ref.extractall(extract_to)
        print(f"Contents extracted to {extract_to}")
        return folder_name
    except zipfile.BadZipFile:
        print("Error: The file is not a valid ZIP file.")
    except Exception as e:
        print(f"Error: {e}")

    return None


__all__ = ["print", "extract_zip"]
