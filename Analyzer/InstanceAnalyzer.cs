using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace Analyzer {
    // zero-based consecutive integer identifier.
    using ID = Int32;
    // the unit of width and height.
    using Length = Int32;
    // the unit of x and y coordinates.
    using Coord = Int32;
    // the unit of elapsed computational time.
    using Duration = Int32;
    // number of neighborhood moves in local search.
    using Iteration = Int32;


    public class InstanceAnalyzer {
        public const string InstanceDir = "Instance/";

        public const ID InvalidItemId = -1;


        public static void analyzeAllInstances(string outputPath) {
            for (int i = 1; i <= 20; ++i) {
                InstanceAnalyzer instAnalyzer = new InstanceAnalyzer();
                instAnalyzer.analyzeInstance("A", i);
            }
        }

        public void analyzeInstance(string dataset, int index) {

        }
    }
}
