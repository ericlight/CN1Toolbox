package main

import (
	"archive/zip"
	"fmt"
	"io"
	"log"
	"os"
)

func main() {
	args:=os.Args

	if len(args)>1 {
		fileName:=args[1]
		_, err := os.Stat(fileName)
		if os.IsNotExist(err) {
			fmt.Print("File not found")
			return
		}

		zipReader, err := zip.OpenReader(fileName)
		checkError(err)

		for _, file := range zipReader.Reader.File {
			zippedFile, err := file.Open()
			checkError(err)

			extractedFile, err := os.Create(file.Name)
			checkError(err)

			_, err = io.Copy(extractedFile, zippedFile)
			checkError(err)

			_ = extractedFile.Close()
			_ = zippedFile.Close()
		}
		fmt.Print("Extraction finished")
		_ = zipReader.Close()

		err = os.Remove(fileName)
		checkError(err)
	}
}

func checkError(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
