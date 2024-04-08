using System;
using System.Collections.Generic;

namespace Experiments
{
  public static class MyMath
  {
    /// <summary>
    /// The evil C# does not have any routine for the median calculation
    /// So we should invite our own bicycle
    /// </summary>
    public static double Median(double[] arr, int lo, int hi)
    {
      void Swap(double[] _arr, int _a, int _b)
      {
        if (_a != _b)
        {
          var _tmp = _arr[_a];
          _arr[_a] = _arr[_b];
          _arr[_b] = _tmp;
        }
      }

      // Prepare a buffer with the requested values, check for the trivial cases
      if (hi <= lo || lo < 0 || hi > arr.Length)
      { return Double.NaN; }

      int size = hi - lo;
      double[] buf = new double[size];
      Array.Copy(arr, lo, buf, 0, size);

      // Use some classic algorithm that I stole from stackoverflow
      int beg = 0;
      int end = size - 1;
      int mid = end / 2;
      while (true)
      {
        var pivot = buf[end];
        var pivotIndex = beg - 1;
        for (var i = beg; i < end; i++)
        {
          if (buf[i] <= pivot)
          { Swap(buf, i, ++pivotIndex); }
        }
        Swap(buf, end, ++pivotIndex);

        if (pivotIndex == mid)
        { return buf[pivotIndex]; }

        if (mid < pivotIndex)
        { end = pivotIndex - 1; }
        else
        { beg = pivotIndex + 1; }
      }

      // No return due to 'while (true)'
    }

    public static double Median(double[] arr)
    { return Median(arr, 0, arr.Length); }
  }
}
