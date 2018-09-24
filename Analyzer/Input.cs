using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;


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


    public class Input {
        public class Item {
            public ID id;
            public Length width;
            public Length height;
            public ID stack;
            public ID seq;
        }

        public class Defect {
            public ID id;
            public ID plateId;
            public Coord x;
            public Coord y;
            public Length width;
            public Length height;
        }

        public class Param {
            public ID plateNum;
            public Length plateWidth;
            public Length plateHeight;
            public Length minL1Width;
            public Length maxL1Width;
            public Length minL2Height;
            public Length minWasteWidth;
            public Length minWasteHeight;
        }


        public enum BatchColumn { ItemId, ItemWidth, ItemHeight, Stack, Seq }
        public enum DefectsColumn { DefectId, PlateId, DefectX, DefectY, DefectWidth, DefectHeight }

        public const char CsvDelimiter = ';';


        public void loadParam() {
            // EXTEND[szx][9]: load from file when provided?
            param.plateNum = 100;
            param.plateWidth = 6000;
            param.plateHeight = 3210;
            param.minL1Width = 100;
            param.maxL1Width = 3500;
            param.minL2Height = 100;
            param.minWasteWidth = 20;
            param.minWasteHeight = 20;
        }
        public bool loadBatch(string bathFilePath) {
            if (!File.Exists(bathFilePath)) { return false; }

            string[] rows = File.ReadAllLines(bathFilePath);
            for (int r = 1; r < rows.Length; ++r) {
                string[] columns = rows[r].Split(CsvDelimiter);
                batch.Add(new Item {
                    id = int.Parse(columns[(int)BatchColumn.ItemId]),
                    width = int.Parse(columns[(int)BatchColumn.ItemWidth].Split('.').First()),
                    height = int.Parse(columns[(int)BatchColumn.ItemHeight].Split('.').First()),
                    stack = int.Parse(columns[(int)BatchColumn.Stack]),
                    seq = int.Parse(columns[(int)BatchColumn.Seq])
                });
            }

            return true;
        }
        public bool loadDefects(string defectFilePath) {
            if (!File.Exists(defectFilePath)) { return false; }

            string[] rows = File.ReadAllLines(defectFilePath);
            for (int r = 1; r < rows.Length; ++r) {
                string[] columns = rows[r].Split(CsvDelimiter);
                defects.Add(new Defect {
                    id = int.Parse(columns[(int)DefectsColumn.DefectId]),
                    plateId = int.Parse(columns[(int)DefectsColumn.PlateId]),
                    x = int.Parse(columns[(int)DefectsColumn.DefectX].Split('.').First()),
                    y = int.Parse(columns[(int)DefectsColumn.DefectY].Split('.').First()),
                    width = int.Parse(columns[(int)DefectsColumn.DefectWidth].Split('.').First()),
                    height = int.Parse(columns[(int)DefectsColumn.DefectHeight].Split('.').First())
                });
            }

            return true;
        }
        public bool load(string bathFilePath, string defectFilePath) {
            loadParam();
            return (loadBatch(bathFilePath) && loadDefects(defectFilePath));
        }


        public List<Item> batch = new List<Item>();
        public List<Defect> defects = new List<Defect>();
        public Param param = new Param();
    }
}
