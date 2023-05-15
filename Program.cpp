using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

class Program
{
    static int MAX_CHARS = 256;

    static void Main()
    {
        string fileName = "input.txt"; // Имя файла для чтения

        Console.WriteLine("Введите строку для поиска:");
        string pattern = Console.ReadLine();
        Stopwatch clock = new Stopwatch();
        clock.Start();
        string filePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, fileName);

        if (!File.Exists(filePath))
        {
            Console.WriteLine("Файл не найден!");
            return;
        }

        using (StreamReader reader = new StreamReader(filePath))
        {
            string text = reader.ReadToEnd();

            List<int> indexes = SearchBoyerMoore(text, pattern);

            if (indexes.Count == 0)
            {
                Console.WriteLine("Подстрока не найдена.");
            }
            else
            {
                Console.WriteLine("Подстрока найдена в следующих позициях:");
                foreach (int index in indexes)
                {
                    int line = GetLineFromIndex(text, index);
                    int position = GetPositionFromIndex(text, index);
                    Console.WriteLine("Строка: " + line + ", Позиция: " + position);
                    Console.WriteLine(text);
                    Console.WriteLine(new string(' ', position -1) + new string('^', pattern.Length));

                }
            }
        }
        clock.Stop();
        Console.WriteLine("Затрачено времени: {0} ", clock.Elapsed);

        Console.ReadLine();
    }

    static List<int> SearchBoyerMoore(string text, string pattern)
    {
        int m = pattern.Length;
        int n = text.Length;

        Dictionary<char, int> badChar = new Dictionary<char, int>();
        int[] goodSuffix = new int[m + 1];

        BadCharHeuristic(pattern, m, badChar);
        GoodSuffixHeuristic(pattern, m, goodSuffix);

        int s = 0; // сдвиг строки

        List<int> indexes = new List<int>();

        while (s <= (n - m))
        {
            int j = m - 1;

            while (j >= 0 && pattern[j] == text[s + j])
                j--;

            if (j < 0)
            {
                indexes.Add(s);
                s += Math.Max(1, goodSuffix[0]);
            }
            else
            {
                int badCharShift = 0;
                if (j < m - 1 && badChar.ContainsKey(text[s + j]))
                    badCharShift = j - badChar[text[s + j]];

                int goodSuffixShift = GoodSuffixShift(j, m, goodSuffix);

                s += Math.Max(1, Math.Max(badCharShift, goodSuffixShift));
            }
        }

        return indexes;
    }

    static void BadCharHeuristic(string str, int size, Dictionary<char, int> badChar)
    {
        for (int i = 0; i < size; i++)
            badChar[str[i]] = i;
    }

    static void GoodSuffixHeuristic(string str, int size, int[] goodSuffix)
    {
        for (int i = 0; i <= size; i++)
            goodSuffix[i] = 0;

        int[] suff = new int[size + 1];

        suff[size - 1] = size;
        goodSuffix[size] = size;

        for (int i = size - 2; i >= 0; i--)
        {
            if (suff[i + 1] > 0)
                suff[i] = suff[i + 1] - 1;
            else
            {
                int j = i + 1;
                while (j < size && str[i] != str[j])
                {
                    if (suff[j] == 0)
                        suff[j] = size - 1 - i;

                    j++;
                }
                suff[i] = j - i;
            }
        }

        for (int i = 0; i < size; i++)
            goodSuffix[i] = size;

        int k = 0;
        for (int i = size - 1; i >= 0; i--)
        {
            if (suff[i] == i + 1)
            {
                while (k < size - 1 - i)
                {
                    if (goodSuffix[k] == size)
                        goodSuffix[k] = size - 1 - i;

                    k++;
                }
            }
        }

        for (int i = 0; i <= size - 2; i++)
            goodSuffix[size - 1 - suff[i]] = size - 1 - i;
    }
    static int GoodSuffixShift(int j, int m, int[] goodSuffix)
    {
        return m - goodSuffix[j + 1];
    }

    static int GetLineFromIndex(string text, int index)
    {
        string substring = text.Substring(0, index);
        return substring.Split('\n').Length;
    }

    static int GetPositionFromIndex(string text, int index)
    {
        int lastNewLineIndex = text.LastIndexOf('\n', index);
        return index - lastNewLineIndex;
    }
}
