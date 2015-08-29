;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

((c-mode
  (c-file-style . "Linux"))
 (c++-mode
  (c-file-style . "Linux")
  (flycheck-clang-include-path
   "../../Database/headers"
   "../../Deserialization/headers"
   "../../ExampleMetricsFeaturesData/headers"
   "../../PDBCatalog/headers"
   "../../PDBData/headers"
   "../../PDBFeatureType/headers"
   "../../PDBMetric/headers"
   "../../PDBTopKQuery/headers")
  (flycheck-clang-language-standard . "c++0x")))

;; Local variables:
;; flycheck-disabled-checkers: 'emacs-lisp
;; End:
