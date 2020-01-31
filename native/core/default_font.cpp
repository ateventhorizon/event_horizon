#include "default_font.hpp"

#include <string>
#include <core/http/basen.hpp>

const std::string defaultFont64 = "AAEAAAAPAIAAAwBwR0RFRgARAJ0AADV0AAAAFkdQT1MDgC91AAA1jAAABABHU1VC3ELqOwAAOYwAAABcT1MvMoQ3HvAAAC+wAAAAYGNtYXBO11LoAAAwEAAAANRnYXNwAAAAEAAANWwAAAAIZ2x5ZrRHr/cAAAD8AAAqiGhlYWT5kxZGAAAs4AAAADZoaGVhBsADJgAAL4wAAAAkaG10eDaOIMAAAC0YAAACdGxvY2EQjxuMAAArpAAAATxtYXhwAOQAQgAAK4QAAAAgbmFtZUQ3ZzIAADDsAAADDHBvc3RD6roYAAAz+AAAAXNwcmVwaAaMhQAAMOQAAAAHAAIAUv/3AMkCcwADAAsAABMzAyMWJjQ2MhYUBmNTCUAGISEzIyMCc/49uSEvISEvIQAAAgAcAc0BFgKpAAMABwAAEwcjJzMHIyduCzwL+gs8CwKp3Nzc3AACADUAHwJKAlEAGwAfAAAlIwcjNyM3MzcjNzM3MwczNzMHMwcjBzMHIwcjJzM3IwF8mg5KDmMFZBNlBmQPSg+aD0kPZwZmE2cGZw5KhZkTmaqLizayNYqKioo1sjaLwbIAAAMARf+lAjMCyQAfACYALQAAARYWFAYHFSM1JiY1MxQXNScmJjQ2NzUzFRYWFyMmJxUXNCcnFTY2AgYUFhcXNQGhRU1yZj5feU2LIFNVZWM+V14LThFhgFwkP0H3OigtHgFKF02FZQZRUghnV3EN6wwaSIZZB0tMB1ZEUAzXukodDNwFPgGyNkkpDgvHAAAFAD3/9QKbAoAACQATABcAIQAtAAASFhUUBiImNTQ2FiYiBhUUFjI2NSUzASMBMhYUBiMiJjQ2FyIGFRQWMzI2NTQm8UxLbElKdiQ6JCQ6JAEZUv5ZUQGqNktLNjZJSTYdJCQdHSQkAoBMRERLTENDTWEsLC8vKiovhf2LARRLiUtMh0w1LC8vKiovLywAAwBZ//UDZQJ/ACQAKwA3AAABBgcXFhYyNjczBgYjIiYnJwYjIiY0NjcnJjU0NjIWFAYHFzY3BAYUFjI3Jzc0JiIGFBYXFzY3NgLVLUYrFB8xJwNKBENAJ0InGmSAaY5fXA5Naa1nTlieRCz+WklTrFG6mzdjNxoeG2gQBgFeiVUlERAlLUhPICMWWWmUSw8NQ1A/VFR3ViaLUn1mNmBFQqPIJDA3QDIaGTA0EQAAAQAbAckAbQKlAAMAABMHIydtCzwLAqXc3AABAEn/jwFIAtgACQAAEhAXByYmNDY3F5utOWJkZGI5AgD+Zq8oWdjn11opAAEAHP+PARsC2AAJAAA2ECc3FhYUBgcnya05YmRkYjlmAZqvKVrX59hZKAAAAQA4AYQBZgKsAA4AABMnNxcnMwc3FwcXBycHJ611GGoKPgpqGHRaLFBQLQIEKDY2gIA2NihZJ2hoJwAAAQASAAAB/QHrAAsAAAEVMxUjFSM1IzUzNQEvzs5Pzs4B69RD1NRD1AAAAQAe/5MAsgBhAAsAABYmNDYyFhQGByc2N1IeHzkmLC07Pg0DGiweLURHFh0eLQAAAQAmAQEBNgFFAAMAABMhFSEmARD+8AFFRAABAB7/9wCUAGgABwAAFiY0NjIWFAY/ISEzIiIJIS8hIS8hAAABADn/twGEAtsAAwAAATMDIwE0UPxPAtv83AAAAgAz//UCIAKAAAcADwAAABYQBiImEDYEJiIGFBYyNgGchITkhYUBFlShVFShVAKAqP7FqKYBP6bLg4P1g4MAAAEAMQAAAOYCdwAIAAATNjczESMRBgcxQzJATjI1AkQJKv2JAhkgAwABAEAAAAIBAoAAHAAANyEVITU0NzY3NzY1NCYiBgcjNjYyFhQGBwcGBhWPAW3+RFQbIXxgQndLBlcFer94T0V/MC9JSV1vNBENMiZXMD0yMlJYaJlaGzMUMykAAAEANP/1AhECgAAhAAABFAcWFhUUBiImNTMWFjI2NCYjIzUzMjY0JiIGByM2NjIWAfluPEp+zpFSAluMTUpTSDFIVERwSw1SC3S3fQHUZCcRTj1TZWthQERDYz8/OWA+JzVTUWEAAAIAGgAAAhkCfgAKAA0AAAERMxUjFSM1ITUBAzMRAaxtbU3+uwFJ6+kCfv5eRJiYPQGp/lcBLAAAAQBB//UB/QJ1ABgAACU0IyMTIRUhBzYzMhYUBiImNTMUFxYzMjYBq5upGwFd/u8RNC9ndnzEfE9UGh9ATsx3ATJItQlbuHljV1IZB1EAAgAz//QCAwKAABUAHgAAASYiBhUUFzY2MhYUBiMiJhA2MzIWFwQGFBYyNjU0JgGpEbRhBQZlnnJ2YHCKjHlUagj+90tKcktLAdZilIA1I1xTb61rtAEls15MoUduQ0I6OkIAAAEAGAAAAcQCdQAGAAABFQEjASE1AcT+4GoBLv6wAnVJ/dQCKUwAAwA3//UCMQKAABUAHwAqAAATJjU0NjIWFRQGBxcWFhUUBiImNTQ2FwYGFBYyNjQmJycUFhcXNjY0JiIGyX18zYRFQxFFSofljlGMP0xgm1swKuYsKUk8TlKCVAE+OWJLXGJNNFIKBhpHPUxcZk0+ThMHQWA8NEotD9gbJxAcAjtfOzkAAgBC//UCEgKAABwAJQAAATIWFxYVFAcGIyImNTMWFjI3NjU0JwYGIiY1NDYWIgYVFBYyNjQBGjNcIUiILzdneFMCRGggWwUKYZ1ydphySktxSwKALClYjfVFF3BVOUQTOcgpIVFQb1ZXaklCOjpCR24AAgA4ABcArgG6AAcADwAANiY0NjIWFAYCJjQ2MhYUBlkhITMiIjMhITMiIhchLyEhLyEBMiEvISEvIQAAAgA5/5MAzQG6AAsAEwAAFiY0NjIWFAYHJzY3AiY0NjIWFAZtHh85JiwtOz4NESEhMyIiAxosHi1ERxYdHi0BTiEvISEvIQAAAQAoACgBmAI8AAYAAAEFBQclNSUBmP7gASA0/sQBPAIAz8w950XoAAIATwB1AZkBeQADAAcAACUVITUlFSE1AZn+tgFK/ra5RETARUUAAQAvACgBngI8AAYAABMFFQUnJSViATz+xDMBHv7iAjzoRec9zM8AAAIAHv/1AcUCgAAZACMAABMiByM2NjIWFRQGBwYGBwYVFSM1ND4CNCYDMhYUBiMiJjQ2+HURVAdsw3E0OhkiChNOKm4mP1caIiIaGiAgAjhcSlpgRTRFIA4WDhkvJy0/QzwtTDP+LiIvICAvIgAAAgBD/0sDfgKAAAoAPQAAATQjIgYVFBYyNjcXFDMyNjc2NTQmIyIGBwYQFjMVIicmJjQ+AjIeAhUUBiMiJicnBgYjIiY1NDYzMhYVAj87O143UjQXQicULxMuootShi9k1LnVhj9EQnajto5kOIFWKzQFAxI8KEpfhFs8RQEhVGFKNTovLS0zGxo9X3eaOC9k/ta5QHM2lbKYbz42XX5DepYkIhAmMGJLY4dISQAAAgAaAAACJQJ1AAcACgAAEzMTIycjByM3MwPyYNNSNvo2U57RaAJ1/YugoN4BNgAAAwBlAAACKQJ1AA8AGAAgAAATMzIWFRQGBxYWFRQHBiMjNzI2NTQmIyMVETMyNjQmIyNl62NrOzw7R3EqPezqSD5EQpuaOkVDPJoCdV5HNlAOClI0dCkPRTowMDnTARc5ZDgAAAEAM//1AjYCgAAWAAABMhYXByYmIyIGFBYzMjY1MxQGIiYQNgE/ZXsXVRRVOU9oXVpLV1WH65GWAoBlURE7RYTrjk9BY3S0AS6pAAACAGQAAAJWAnUABwAQAAABMhYQBiMjERMzMjY1NCYjIwEdjqurjrlPaml8fWhqAnWr/uGrAnX90Itra4kAAAEAZAAAAecCdQALAAATIRUhFSEVIRUhFSFkAYP+zAEi/t4BNP59AnVE00PWRQABAGUAAAHjAnUACQAAEyEVIRUhFSERI2UBfv7RARr+5k8CdUTRRP7kAAABADP/9QI7An8AGQAAJQYiJhA2MzIWFwcmIgYUFjMyNjU1JzUzESMB6irym6l+Sm8oTEGueWhYSFie60Bsd7ABKrA8QhpRjuOKYF4CBjT+yQAAAQBlAAACOAJ1AAsAABMRIREzESMRIREjEbQBNU9P/stPAnX+6wEV/YsBHP7kAnUAAQBlAAAAtAJ1AAMAABMzESNlT08Cdf2LAAABAAj/9QFEAnUACwAAJAYiJzUWMjY1ETMRAURdkE9GeyxPT1oqUjA8OQG//kEAAQBlAAACTAJ1AAsAABMRATMBASMDBxUjEbUBGW3+7QEkZvM+UAJ1/t0BI/7l/qYBI0DjAnUAAQBlAAABzAJ1AAUAADchFSERM7QBGP6ZT0VFAnUAAQBlAAAC6wJ1AAwAABsCMxEjEQMjAxEjEdbU0m9P2DXbTwJ1/lYBqv2LAgX+SgG9/fQCdQAAAQBjAAACSAJ1AAkAABMBETMRIwERIxHBAThPWP7DUAJ1/gcB+f2LAgD+AAJ1AAIAM//1AlQCgAAKABQAAAEyFhAGIyImNTQ2FyIGFBYzMjY0JgFDepeWe3uVlXtYZmZYWGdnAoCn/sGlp56fp0eE+ICA+IQAAAIAYwAAAgMCdQAJABEAAAAWFAYjIxUjETMRMjY0JiMjFQGWbW1ofE/LQUNDQXwCdWmyZ/MCdf7CQnNF+gAAAgAz/1kCVAKAABUAHwAABQYjIicnJiYQNjMyFhUUBgcXFhcyNwEiBhQWMzI2NCYCVCgsVT0veZOVe3qXalsfHSgrNv7vWGZmWFhnZ5QTWUMCpgE8p6efhKEXLCcCGQJ3hPiAgPiEAAACAGUAAAIrAnUADAAUAAAABgcTIwMjESMRMzIWBzI2NCYjIxUCG1NRtGajbk/banHhR0lHQ4wBc2AO/vsBAf7/AnVm10VxQ/kAAAEARf/1AhgCgAAfAAABJiMiBhQWFxcWFhQGIiYnMxQWMjY0JicnJiY0NjIWFwG0EXY/RCovg0NPfNCFAk1XiVAxMG9OUHDKawgB3Fw3TSgPLRdQkWNpXz5CPVMwECkaSItdWUsAAAEABgAAAe8CdQAHAAATIRUjESMRIwYB6c1PzQJ1RP3PAjEAAAEAV//1AjUCdQAQAAATERQWMzI2NREzERQGIiY1EaZYSEhYT4LbgQJ1/m5YTk5YAZL+bnZ4eHYBkgABABIAAAIhAnUABgAAGwIzAyMDZbS1U99S3gJ1/e8CEf2LAnUAAAEADgAAA3ACdQAMAAAbAjMTEzMDIwMDIwNmmZs1mrBX3lOJjlPHAnX99QHK/jYCC/2LAZP+bQJ1AAABAC8AAAJRAnUACwAAISMnByMTAzMTEzMDAlFitqlg3t9ftKZf2fv7ATgBPf7/AQH+xAAAAQAKAAACGwJ1AAgAABsCMwMRIxEDaKmtXeJQ3wJ1/t8BIf6N/v4BAgFzAAABAEkAAAIXAnUACQAANwElNSEVAQUVIUkBa/6VAc7+lQFr/jJHAeMDSEf+HQNIAAABAGX/mgErAtsABwAAEzMVIxEzFSNlxnd3xgLbRv1LRgAAAQAt/7cBeALbAAMAABMzEyMtT/xQAtv83AAB//L/mgC4AtsABwAAFyM1MxEjNTO4xnd3xmZGArVGAAEAFQD1AbICSgAGAAA3EzMTIycHFbA9sFh3dfUBVf6r9vYAAAEAN/+EAp//yAADAAAXIRUhNwJo/Zg4RAAAAQAYAqUBJANcAAMAABMXByc56xf1A1yFMnEAAAIARv/1AdsB6gAYACEAAAE0JiIGFSM0NzYzMhYVESMnBiMiJjQ2MhcVJiIGFRQzMjYBjTZnP1d2JCpZZEQILHNNXWeaRkdqQmY6UwE2QTgpLG4aCFpc/sxQW1aASBoyFCYpWlMAAAIAZf/1AhYCqQANABcAABM2MhYUBiMiJicHIxEzEgYHFRQWMjY0JrQwuHp8WTBNFhI3T05MAk9wUlEBl1OJ4YswL1QCqf77SUFOQk9jpGIAAQAy//UBzQHqABUAAAAWFwcmJiMiBhQWMjY1MxQGIyImNDYBWWQPUA06I0BPTXM5UWtWW39+AepXQQcqL2ClZDUxTV+K5YYAAgA8//UB7AKpAA0AFwAAAREzESMnBgYjIiY0NjIGBhQWMjY3NTQmAZ5OQAoXTC1be3m5kk9QcE8CTgGXARL9V1AtLozhiEZipGNLP05DTgACADL/9QHXAeoAEAAYAAAlMjczBgYjIiY0NjIWByEUFhMmIyIGBzM0AQpjDlELaU5geHnJYw/+uUh7FyE9Rwb/NVdIT4nkiJxwSl8BagxTPmQAAQAxAAABWwKxABQAAAEmIyIGFRUzFSMRIxEjNTM1NDYyFwFNIRMjKGNjTk9PTmYnAmYNIitINv5YAag2SUdDDQAAAwAc/woCJQI7ACgANAA+AAA3JjU0NjcmNTQ2Mhc2NjMHIgcWFRQGIyInBgYVFDMzMhYVFAYiJjU0NhcUFjI2NTQmIyMGBgAmIgYVFBYzMjZ5PC8jNm+YNQdHOgdYBCNvUzksFCBYukNOmeqGMxtTqXEhIsEzNgEmPm0+PzY2Pg8dPB8vCDBOTmAkNUBOSis8TWMXBB0ZNTkzTHJXQis2XiY3STEXIQItAaRBQTQ0Q0QAAAEAZQAAAgcCqQARAAATNjMyFhURIxE0JiIGFRUjETO0MG9VX05BekpPTwGGZHBh/ucBGUVGZ1viAqkAAAIAXgAAAMYCsgADAAsAABMzESMSJjQ2MhYUBmpPTw8bGzAdHQHf/iECTxstGxstGwAAAv+q/woA1QKyAAkAFwAAEiY0NjMyFhQGIwMyNREzERQHBiMiJzcWiBsbGBgdHRh/WE9aHCI+SAQ/Ak8bLRsbLRv9AG4CI/3diSAKJUUlAAABAGUAAAIUAqkACwAANwcVIxEzETczBxMj9EBPT+du3Odh/TTJAqn+dMGz/tUAAQBlAAAAtAKpAAMAABMzESNlT08Cqf1XAAABAGQAAANSAeoAHgAAEzYzMhYXNjMyFhURIxE0JiIGFRUjETQmIgYHFSMRM7Evcj5UEy16Vl5OQXpDTkF4SgJPRAGCaD02c3Bh/ucBGUVGZFjoARlFRmJX6wHeAAEAZAAAAgYB6gARAAATNjMyFhURIxE0JiIGBxUjETOxL3JVX05BeEoCT0QBgmhwYf7nARlFRmJX6wHeAAIAMv/1AfAB6gALABMAAAEyFhUUBiMiJjU0NhYmIgYUFjI2ARFke3tkZHt780iLSkaLTAHqgnl4goJ4eYKlYWGrYWEAAgBl/xUCFgHqAAsAFQAAEzYyFhQGIicRIxEzEjY0JiIGBxUUFq8vu31+uCxPQcpVUXRKAkwBlVWK4olF/tsCyf5dYKdiST5bP0gAAAIAM/8KAiwB6gATAB8AAAEyFzczERQWFhcHJiY1NQYiJjQ2FgYUFjMyNzY3NTQmAQhmLhE2Fx0VGkQ5Kbt+fSVRVjlSIwwBTAHqWk790TEhDwc9Ek5KmFeJ4opGYqdgSRcgW0JMAAEAZAAAAWkB5wANAAABIgcVIxEzFTYzMhcHJgEocQRPTyRbHRoEHgGbsukB31xkCU0KAAABADz/9QHRAeoAHwAAEjYyFhcjJiMiBhQWFxcWFhQGIiYnMxYWMjY0JicnJjVMZaxhAk0JZTE1KCxgPUFqvmoDTAJFaEQoLV19AaRGSDdBLDMjDSATO2dTVUMtLSw9IAwfJV0AAQAi/+YBXwJjABUAACUGJyYnJjURIzUzNTMVMxUjERQzMjcBX1ZHMxULTU1OlpZEJS4OKBcQMxkjASY8hYU8/ttJEgABAFf/9wH5Ad8AEQAAJQYjIiY1ETMRFDMyNjc1MxEjAaoxblFjT3BBUQJPT19oWl4BMP7WeWRO8f4hAAABABIAAAHlAd4ABgAAGwIzAyMDa5CRWb5XvgHe/nUBi/4iAd4AAAEAEgAAAsAB3gAMAAAbAjMTEzMDIwMDIwNkdHJCcXVOnE1ubEyfAd7+gwFm/pYBgf4iAU/+sQHeAAABABoAAAHmAd8ACwAAISMnByM3JzMXNzMHAeVgiYJgtbVgiYNgt7W17/C0tPAAAQAF/woBywHeABEAABcWMzI3Njc3AzMTEzMDBgYiJwUxNigdFREOxFiRcVCzF0tzPosmKB07MQHe/oMBff3DSU4jAAABADUAAAGnAd4ACQAANwEhNSEVASEVITUBGf7nAXL+6wEV/o5AAV5AQP6iQAAAAQBL/3YBVQLsAB8AABMWFRUUFjMVIjU1NCYjIzUzMjY1NTQ3NjMVIgYVFRQHmjQ9StURExERExFyKDtKPTQBLxlYXVFVRepcKyRMIyxbqDESRVVSXFYbAAEAZf+GALQC7gADAAATMxEjZU9PAu78mAAAAQAI/3YBEgLsAB8AABMmNTU0JiM1MhUVFBYzMxUjIgYVFRQHBiM1MjY1NTQ3wzQ9StURExERExFyKTpKPTQBMxtWXFJVRetbLCNMJCtcqDASRVVRXVgZAAEANQDfAc4BZwARAAATIgcnNjMyFjMyNxcGIyImJyaeJRgsIUoihhooGSsjThdgDicBGjsXbzc5G2okBg4AAQAUAAACEQKAACUAAAEmIyIGFRUzFSMVFAczMjY3MwYGIyE1MzI2NTUjNTM1NDYzMhYXAZAQVio5trYltzEtBlQGVF7+uxwmI1tbXVtHWgsB6k5ASG04aEQaKSxNTUUsLmw4bWNtTEoAAAIAGAMLASoDbwAJABEAABImNDYzMhYUBiMyJjQ2MhYUBjUdHRcXHx8Xkh4eLR8fAwsdKh0dKh0dKh0dKh0AAQAXAqUBIgNcAAMAAAEHJzcBIvQX6wMWcTKFAAMAGgAAAiUDXAAHAAoADgAAEzMTIycjByM3MwMDFwcn8mDTUjb6NlOe0Whj6xf1AnX9i6Cg3gE2AUiFMnEAAwAaAAACJQNcAAcACgAOAAATMxMjJyMHIzczAxMHJzfyYNNSNvo2U57RaIb0F+sCdf2LoKDeATYBAnEyhQADABoAAAIlA2gABwAKABAAABMzEyMnIwcjNzMDNwcnByc38mDTUjb6NlOe0Wi0MoGCMrQCdf2LoKDeATa4J2xsJ5wAAAMAGgAAAiUDSgAHAAoAHgAAEzMTIycjByM3MwMnIgcnNjYyFxYWMjY3FwYGIi4C8mDTUjb6NlOe0WhEIBkgDC40Ii4bGhwKIwsxKSI8FQJ1/YugoN4BNu4zEzM1ERcMGRsXMDQMHwgABAAaAAACJQNDAAcACgAUABwAABMzEyMnIwcjNzMDJiY0NjMyFhQGIzImNDYyFhQG8mDTUjb6NlOe0WhWHR0XFx8fF5IeHi0fHwJ1/YugoN4BNssdKh0dKh0dKh0dKh0AAgBkAAAB5wNcAAsADwAAEyEVIRUhFSEVIRUhExcHJ2QBg/7MASL+3gE0/n1e6xf1AnVE00PWRQNchTJxAAACAGQAAAHnA1wACwAPAAAzITUhNSE1ITUhNSElByc3ZAGD/swBIv7eATT+fQFH9BfrRdZD00ShcTKFAAACAGkAAAHsA2gACwARAAATIRUhFSEVIRUhFSEBBycHJzdpAYP+zAEi/t4BNP59AXYygYIytAJ1RNND1kUCzCdsbCecAAMAZAAAAecDQwALABUAHQAAEyEVIRUhFSEVIRUhEiY0NjMyFhQGIzImNDYyFhQGZAGD/swBIv7eATT+fVcdHRcXHx8Xkh4eLR8fAnVE00PWRQLfHSodHSodHSodHSodAAIABwAAARMDXAADAAcAABMzESMDFwcnZU9PPesX9QJ1/YsDXIUycQACAAYAAAERA1wAAwAHAAAzMxEjNwcnN2VPT6z0F+sCdaFxMoUAAAL/2QAAAUADaAADAAkAABMzESMTBycHJzdlT0/bMoGCMrQCdf2LAswnbGwnnAADAAQAAAEWA0MAAwANABUAABMzESMCJjQ2MzIWFAYjMiY0NjIWFAZlT09EHR0XFx8fF5IeHi0fHwJ1/YsC3x0qHR0qHR0qHR0qHQAAAgBjAAACSANKAAkAHQAAExEzEQEzESMRATciByc2NjIXFhYyNjcXBgYiLgJjUAE9WE/+yFEgGSAMLjQhLxsaHAojCzEpIjwVAnX9iwIA/gACdf4HAfmNMxMzNREXDBkbFzA0DB8IAAADADP/9QJUA1wACgAUABgAAAEyFhAGIyImNTQ2FyIGFBYzMjY0JgMXBycBQ3qXlnt7lZV7WGZmWFhnZ7zrF/UCgKf+waWnnp+nR4T4gID4hAEjhTJxAAMAM//1AlQDXAAKABQAGAAAASIGFRQWMzI2ECYHMhYUBiMiJjQ2NwcnNwFDe5WVe3uWl3pYZ2dYWGZm3fQX6wKAp5+ep6UBP6dHhPiAgPiE3XEyhQAAAwAz//UCVANoAAoAFAAaAAABMhYQBiMiJjU0NhciBhQWMzI2NCY3BycHJzcBQ3qXlnt7lZV7WGZmWFhnZ1sygYIytAKAp/7Bpaeen6dHhPiAgPiEkydsbCecAAADADP/9QJUA0oACgAUACgAAAEiBhUUFjMyNhAmBzIWFAYjIiY0NjciByc2NjIXFhYyNjcXBgYiLgIBQ3uVlXt7lpd6WGdnWFhmZhMgGSAMLjQhLxsaHAojCzEpIjwVAoCnn56npQE/p0eE+ICA+ITJMxMzNREXDBkbFzA0DB8IAAQAM//1AlQDQwAKABQAHgAmAAABMhYQBiMiJjU0NhciBhQWMzI2NC4CNDYzMhYUBiMyJjQ2MhYUBgFDepeWe3uVlXtYZmZYWGdnxB0dFxcfHxeSHh4tHx8CgKf+waWnnp+nR4T4gID4hKYdKh0dKh0dKh0dKh0AAAIAV//1AjUDXAAQABQAABMRFBYzMjY1ETMRFAYiJjURNxcHJ6ZYSEhYT4LbgYvrF/UCdf5uWE5OWAGS/m52eHh2AZLnhTJxAAIAV//1AjUDXAAQABQAABMRFBYyNjURIxEUBiMiJjURJQcnN1eB24JPWEhIWAEl9BfrAnX+bnZ4eHYBkv5uWE5OWAGSoXEyhQAAAgBX//UCNQNoABAAFgAAExEUFjMyNjURMxEUBiImNRElBycHJzemWEhIWE+C24EBojKBgjK0AnX+blhOTlgBkv5udnh4dgGSVydsbCecAAADAFf/9QI1A0MAEAAaACIAABMRFBYzMjY1ETMRFAYiJjURNiY0NjMyFhQGIzImNDYyFhQGplhISFhPgtuBgx0dFxcfHxeSHh4tHx8Cdf5uWE5OWAGS/m52eHh2AZJqHSodHSodHSodHSodAAADADz/9QHRAtsAGAAhACUAAAE0JiIGFSM0NzYzMhYVESMnBiMiJjQ2MhcVJiIGFRQzMjYDFwcnAYM2Zz9XdiQqWWRECCxzTV1nmkZHakJmOlPN6xf1ATZBOCksbhoIWlz+zFBbVoBIGjIUJilaUwJWhTJxAAMAPP/1AdEC2wAYACIAJgAAATQmIgYVIzQ3NjMyFhURIycGIyImNDYyFxUmIgYVFDMyNjcTByc3AYM2Zz9XdiQqWWRECCxzTV1nmkZHakJmOVICHPQX6wE2QTgpLG4aCFpc/sxQW1aASBoyFCYpWlA/AdRxMoUAAAMAPP/1AdEC5wAYACEAJwAAATQmIgYVIzQ3NjMyFhURIycGIyImNDYyFxUmIgYVFDMyNhMHJwcnNwGDNmc/V3YkKllkRAgsc01dZ5pGR2pCZjpTSjKBgjK0ATZBOCksbhoIWlz+zFBbVoBIGjIUJilaUwHGJ2xsJ5wAAwA8//UB0QLJABgAIgA2AAABNCYiBhUjNDc2MzIWFREjJwYjIiY0NjIXFSYiBhUUMzI2NwMiByc2NjIXFhYyNjcXBgYiLgIBgzZnP1d2JCpZZEQILHNNXWeaRkdqQmY5UgKuIBkgDC40Ii4bGhwKIwsxKSI8FQE2QTgpLG4aCFpc/sxQW1aASBoyFCYpWlA/AcAzEzM1ERcMGRsXMDQMHwgABAA8//UB0QLBABgAIQArADMAAAE0JiIGFSM0NzYzMhYVESMnBiMiJjQ2MhcVJiIGFRQzMjYCJjQ2MzIWFAYjMiY0NjIWFAYBgzZnP1d2JCpZZEQILHNNXWeaRkdqQmY6U9UdHRcXHx8Xkh4eLR8fATZBOCksbhoIWlz+zFBbVoBIGjIUJilaUwHYHSodHSodHSodHSodAAADADP/9QHYAuAAEAAYABwAACUyNzMGBiMiJjQ2MhYHIRQWEyYjIgYHMzQDFwcnAQtjDlELaU5geHnJYw/+uUh6FiA+Rwb/4+sX9TVXSE+J5IiccEpfAWoMUz5kAWKFMnEAAAMAM//1AdgC4AAQABYAGgAAJQYjIiY1ITYmIgYUFjMyNjcCFhUjNjY3Byc3AXwOY0FIAUcPY8l5eGBOaQuCN/8GR7j0F+uMV19KcJyI5IlPSAEfUz4+U+9xMoUAAwA2//UB2wLsABAAGAAeAAAlMjczBgYjIiY0NjIWByEUFhMmIyIGBzM0NwcnByc3AQ5jDlELaU5geHnJYw/+uUh7FyE9Rwb/NTKBgjK0NVdIT4nkiJxwSl8BagxTPmTSJ2xsJ5wABAAz//UB2ALBABAAGAAiACoAACUyNzMGBiMiJjQ2MhYHIRQWEyYjIgYHMzQmJjQ2MzIWFAYjMiY0NjIWFAYBC2MOUQtpTmB4ecljD/65SHoWID5HBv/qHR0XFx8fF5IeHi0fHzVXSE+J5IiccEpfAWoMUz5k3x0qHR0qHR0qHR0qHQAAAgAMAAABGALbAAMABwAAEzMRIwMXBydqT0896xf1Ad/+IQLbhTJxAAIACwAAARYC2wADAAcAADMzESM3Byc3ak9PrPQX6wHftnEyhQAAAv/eAAABRQLnAAMACQAAEzMRIxMHJwcnN2pPT9sygYIytAHf/iECSydsbCecAAMACQAAARsCwQADAA0AFQAAEzMRIwImNDYzMhYUBiMyJjQ2MhYUBmpPT0QdHRcXHx8Xkh4eLR8fAd/+IQJdHSodHSodHSodHSodAAACAGUAAAIHAskAEQAlAAABIgcnIxEzNTY2MhYVETMRNCYnIgcnNjYyFxYWMjY3FwYGIi4CAVNyLwlETwJKeEFOX7sgGSAMLjQiLhsaHAojCzEpIjwVAepoXP4i61diRkX+5wEZYXCXMxMzNREXDBkbFzA0DB8IAAADADP/9QHxAtsACwATABcAAAEyFhUUBiMiJjU0NhYmIgYUFjI2AxcHJwESZHt7ZGR7e/NIi0pGi0zz6xf1AeqCeXiCgnh5gqVhYathYQJBhTJxAAADADP/9QHxAtsACwATABcAAAEiBhUUFjMyNjU0JgY2MhYUBiImAQcnNwESZHt7ZGR7e/JKi0hMi0YBE/QX6wHqgnl4goJ4eYKlYWGrYWEB+3EyhQADADP/9QHxAucACwATABkAAAEyFhUUBiMiJjU0NhYmIgYUFjI2EwcnByc3ARJke3tkZHt780iLSkaLTCQygYIytAHqgnl4goJ4eYKlYWGrYWEBsSdsbCecAAADADP/9QHxAskACwATACcAAAEiBhUUFjMyNjU0JgY2MhYUBiImEyIHJzY2MhcWFjI2NxcGBiIuAgESZHt7ZGR7e/JKi0hMi0ZKIBkgDC40IS8bGhwKIwsxKSI8FQHqgnl4goJ4eYKlYWGrYWEB5zMTMzURFwwZGxcwNAwfCAAEADP/9QHxAsEACwATAB0AJQAAATIWFRQGIyImNTQ2FiYiBhQWMjYCJjQ2MzIWFAYjMiY0NjIWFAYBEmR7e2Rke3vzSItKRotM+h0dFxcfHxeSHh4tHx8B6oJ5eIKCeHmCpWFhq2FhAcMdKh0dKh0dKh0dKh0AAgBX//cB+QMBABEAFQAAJQYjIiY1ETMRFDMyNjc1MxEjAxcHJwGqMW5RY09wQVECT0/i6xf1X2haXgEw/tZ5ZE7x/iEDAYUycQACAFf/9wH5AwEAEQAVAAAFMjcVMxEjFRQGIyI1ESMRFBYTByc3AQtuMU9PUkJwT2P39BfrCWhfAd/pU2d5ASr+0F5aAsRxMoUAAAIAV//3AfkDDQARABcAACUGIyImNREzERQzMjY3NTMRIxMHJwcnNwGqMW5RY09wQVECT082MoGCMrRfaFpeATD+1nlkTvH+IQJxJ2xsJ5wAAwBX//cB+QLBABEAGwAjAAAlBiMiJjURMxEUMzI2NzUzESMCJjQ2MzIWFAYjMiY0NjIWFAYBqjFuUWNPcEFRAk9P6R0dFxcfHxeSHh4tHx9faFpeATD+1nlkTvH+IQJdHSodHSodHSodHSodAAABAGUAAAC0Ad8AAwAAEzMRI2VPTwHf/iEAAAEAJAGjALgCcQALAAASJjQ2MhYUBgcnNjdYHh85JiwtOz4NAg0aLB4tREcWHR4tAAEAGAKlAX8DaAAFAAABBycHJzcBfzKBgjK0AswnbGwnnAABABcCzwFTA0oAEwAAEyIHJzY2MhcWFjI2NxcGBiIuAnAgGSAMLjQhLxsaHAojCzEpIjwVAwIzEzM1ERcMGRsXMDQMHwgAAQA3AQEBowFFAAMAABMhFSE3AWz+lAFFRAABADcBAQKfAUUAAwAAEyEVITcCaP2YAUVEAAEAJAGiALgCcgAMAAASBiImNDY3FwYHFhYVoh84Jy8tOD4NFx4BwR8uRUcWHSAsAhoWAAABAA8BogCiAnIADQAAEjYyFhUUBgcnNjcmJjUkIDklLyw4Pg0XHwJUHiwjI0cXHiAsAhoWAAABAFwAAAH8AnUAGwAAEyEVIxYXMxUjBgcGBwUjJTUzMjY3IzUzJiYjI1wBoLdFD2NeB3AoNwEScP7yPVdUA+vnC1NMPQJ1MSRDMHMsDwX6+DBMOTAuOQAAAQBPAQIBmQFFAAMAAAEVITUBmf62AUVDQwABAAAAnQA/AAUAAAAAAAIAAAABAAEAAABAAAAAAAAAAAAAAAAAAAAAAAAZACwAXQCkAOsBQgFPAWUBewGZAa4BxgHTAeUB8wITAicCVQKIAqQCywL9AxADUgOLA6kDzQPhA/QECAQ/BJYErgTgBQYFJQU8BVEFegWSBZ8FtgXRBeAF+wYSBjYGVQaJBq4G4QbzBxAHIwdAB1oHcAeIB5kHpge2B8gH1QfjCBYIPghiCIoIswjUCS0JSwlkCYwJowmwCd4J/AoeCkQKdwqRCsMK5QsDCxYLMwtKC2wLgwuvC7wL6AwHDDwMWwxpDIgMpwzJDP0NLQ1MDWoNjA28DdAN4w36DiAOVA5/DqoO2A8YD1QPeA+dD8UP+xA1EHEQrhD/EUsRfBGqEd0SHxIzEkYSXRKDEr8S6RMTE0ATfxO6E98UBBQsFGMUcBSIFJkUvBTJFNYU8BULFTcVRAABAAAAAQAAuIaAKF8PPPUACwPoAAAAAMq8ex8AAAAAzI9Xyf+q/woDfgNvAAAACAACAAAAAAAAAikAAAAAAAACKQAAAO4AAAEWAFIBMgAcAnMANQJ4AEUC1gA9A5QAWQCIABsBZABJAWQAHAGcADgCDwASANIAHgFdACYAswAeAbAAOQJSADMBSwAxAkQAQAJPADQCLwAaAjUAQQI9ADMB1gAYAmcANwJFAEIA5wA4AQYAOQHGACgB6ABPAcYALwHzAB4DrABDAj8AGgJuAGUCZQAzApUAZAIyAGQCDABlAoUAMwKdAGUBGABlAZQACAJhAGUB1ABlA1AAZQKtAGMChgAzAioAYwKMADMCZQBlAl0ARQH1AAYCjgBXAjMAEgN/AA4CggAvAiUACgJQAEkBPQBlAbAALQEd//IBwAAVAtYANwE7ABgCMABGAkgAZQIDADICUgA8AgMAMgFXADECNQAcAlwAZQErAF4BOv+qAjQAZQEYAGUDpwBkAlsAZAIiADICUgBlAkkAMwF9AGQCDgA8AXgAIgJeAFcB9wASAtUAEgIAABoB5AAFAdsANQFdAEsBGABlAV0ACAIDADUCNQAUAUMAGAE7ABcCPwAaAj8AGgI/ABoCPwAaAj8AGgI0AGQCNABkAjQAaQI0AGQBGAAHARgABgEY/9kBGAAEAq4AYwKGADMChgAzAoYAMwKGADMChgAzAo4AVwKOAFcCjgBXAo4AVwInADwCJwA8AicAPAInADwCJwA8AgIAMwICADMCAgA2AgIAMwErAAwBKwALASv/3gErAAkCXABlAiMAMwIjADMCIwAzAiMAMwIjADMCXgBXAl4AVwJeAFcCXgBXARgAZQC4ACQBlwAYAW4AFwHaADcC1gA3ANYAJADWAA8CRQBcAegATwABAAADlf8EAAADrP+q/9gDfgABAAAAAAAAAAAAAAAAAAAAnQADAf0BkAAFAAgCvAKKAAAAjAK8AooAAAHdADIA+gAAAAAAAAAAAAAAAIAAACcAAABCAAAAAAAAAABweXJzAEAAICISA5X/BAAAA5UA/AAAAAEAAAAAAd4CdQAAACAAAgAAAAIAAAADAAAAFAADAAEAAAAUAAQAwAAAACwAIAAEAAwAfgCgAKMAqAC0AMQAzwDWANwA5ADvAPYA/AExArwCxgLcIBQgGSC5IhL//wAAACAAoACjAKgAtADAAMgA0QDZAOAA6ADxAPkBMQK8AsYC3CATIBgguSIS////4/9j/7//u/+w/6X/ov+h/5//nP+Z/5j/lv9i/dj9z/264ITggd/i3ooAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAuAH/hbAEjQAAAAALAIoAAwABBAkAAADaAAAAAwABBAkAAQAKANoAAwABBAkAAgAOAOQAAwABBAkAAwBUAPIAAwABBAkABAAKANoAAwABBAkABQAaAUYAAwABBAkABgAaAWAAAwABBAkACQAgAXoAAwABBAkADAAcAZoAAwABBAkADQCYAbYAAwABBAkADgA0Ak4AQwBvAHAAeQByAGkAZwBoAHQAIAAoAGMAKQAgADIAMAAxADEALQAyADAAMQAyACwAIABKAG8AbgBhAHQAaABhAG4AIABQAGkAbgBoAG8AcgBuACAAKABqAG8AbgBwAGkAbgBoAG8AcgBuAC4AdAB5AHAAZQBkAGUAcwBpAGcAbgBAAGcAbQBhAGkAbAAuAGMAbwBtACkALAAgAHcAaQB0AGgAIABSAGUAcwBlAHIAdgBlAGQAIABGAG8AbgB0ACAATgBhAG0AZQBzACAAJwBLAGEAcgBsAGEAJwBLAGEAcgBsAGEAUgBlAGcAdQBsAGEAcgBGAG8AbgB0AEYAbwByAGcAZQAgADIALgAwACAAOgAgAEsAYQByAGwAYQAgAFIAZQBnAHUAbABhAHIAIAA6ACAAMQAzAC0AMQAwAC0AMgAwADEAMQBWAGUAcgBzAGkAbwBuACAAMQAuADAAMAAwAEsAYQByAGwAYQAtAFIAZQBnAHUAbABhAHIASgBvAG4AYQB0AGgAYQBuACAAUABpAG4AaABvAHIAbgBqAG8AbgBwAGkAbgBoAG8AcgBuAC4AYwBvAG0AVABoAGkAcwAgAEYAbwBuAHQAIABTAG8AZgB0AHcAYQByAGUAIABpAHMAIABsAGkAYwBlAG4AcwBlAGQAIAB1AG4AZABlAHIAIAB0AGgAZQAgAFMASQBMACAATwBwAGUAbgAgAEYAbwBuAHQAIABMAGkAYwBlAG4AcwBlACwAIABWAGUAcgBzAGkAbwBuACAAMQAuADEALgBoAHQAdABwADoALwAvAHMAYwByAGkAcAB0AHMALgBzAGkAbAAuAG8AcgBnAC8ATwBGAEwAAgAAAAAAAP+1ADIAAAAAAAAAAAAAAAAAAAAAAAAAAACdAAAAAQACAAMABAAFAAYABwAIAAkACgALAAwADQAOAA8AEAARABIAEwAUABUAFgAXABgAGQAaABsAHAAdAB4AHwAgACEAIgAjACQAJQAmACcAKAApACoAKwAsAC0ALgAvADAAMQAyADMANAA1ADYANwA4ADkAOgA7ADwAPQA+AD8AQABBAEIAQwBEAEUARgBHAEgASQBKAEsATABNAE4ATwBQAFEAUgBTAFQAVQBWAFcAWABZAFoAWwBcAF0AXgBfAGAAYQCFAI4AjQCtAMkAxwCuAGIAywBlAMgAygDPAMwAzQDOAGYA0wDQANEArwBnANYA1ADVAGgAagBpAGsAbQBsAHEAcAByAHMAdQB0AHYAdwB4AHoAeQB7AH0AfAB/AH4AgACBANcBAgDYANkAsgCzALYAtwEDAO8KYXBvc3Ryb3BoZQtydXBlZXN5bWJvbAAAAQAB//8ADwABAAAADAAAAAAAAAACAAEAAQCcAAEAAAABAAAACgAeACwAAWxhdG4ACAAEAAAAAP//AAEAAAABa2VybgAIAAAAAQAAAAEABAACAAAAAQAIAAEAUgAEAAAAJACYALoAwAEGASgBNgE8AUIBSAGeAdgCDgIUAoIClAKiArACvgLYAuIC7ALyAwADCgMUAyYDMAM+A0wDZgNsA3YDjAOeA6gDvgACAAsAJAAkAAAAJwAnAAEAKQApAAIALwAvAAMAMgA0AAQANgA3AAcAOQA8AAkARABGAA0ASABLABAATQBOABQAUABdABYACAA3/9gAOf/kADr/5AA8/+YASf/0AFn/6gBa/+gAXP/kAAEAPP/nABEAJP/HADr/+AA7//AAPP/4AET/6ABG/+QAR//oAEj/5ABK/9MAUv/kAFT/5ABY/+gAWf/gAFr/3ABb/+QAXP/kAF3/6AAIAC0AEAA3/88AOf/TADr/1wA8/7sAWf/oAFr/6ABc/+wAAwA5//gAO//0ADz/8AABACT/2gABADz/8AABADz/5gAVACT/2AAt/7sARP+pAEb/tQBH/6sASP+1AEn/ywBK/6kAUP+7AFH/uwBS/7EAU/+7AFT/tQBV/7cAVv+fAFj/uwBZ/7cAWv/YAFv/xwBc/84AXf/LAA4AJP/fACb/+AAt/7sAMv/4AET/3wBG/98AR//fAEj/3wBJ/9gASv/PAFL/3wBU/+MAVf/vAFb/3wANACT/3wAt/7sARP/fAEb/3wBH/98ASP/fAEn/2ABK/88AUv/fAFT/3wBV/98AVv/YAFj/5AABADL/9AAbACT/uwAm/+8AK//4AC3/rgAy//AANP/wADb/5ABE/8MARv+/AEf/vwBI/78ASf+/AEr/swBQ/78AUf+/AFL/vwBT/78AVP+/AFX/vwBW/78AV//bAFj/vQBZ/9MAWv/TAFv/ywBc/8cAXf+/AAQAN/+nADn/2wA6/+QAPP/LAAMAN//sADn/4wA8/+8AAwA3/8MAOf/kADz/3wADADf/wwA5/+gAPP/XAAYAJP/wAC3/ywBG//AASv/mAE3/8ABW/+gAAgBNAEkAXAAQAAIAN//4ADz/0wABAEn/6gADAEn/8ABN//gAXP/wAAIAN//cADz/1wACADf/1AA8/9cABAA3/7kAOf/fADr/3wA8/78AAgA3/8sAPP/cAAMAN/+3ADz/0QBNAFEAAwAt/9gAN//QAEr/7AAGADf/pwA5/+QAOv/cADz/xwBJ/+QAXP/sAAEAPP/bAAIAN//HADz/zwAFACT/6AAt/8sAN//EADz/0wBK//gABAAk/+QALf/XADf/4AA8/+AAAgA3/9cAPP/PAAUAJP/0AC3/6AA3/8gAPP/kAEr/+AACADf/6AA8/9cAAQAAAAoAHgAsAAFsYXRuAAgABAAAAAD//wABAAAAAW9udW0ACAAAAAEAAAABAAQAAQAAAAEACAACABoACgAAAAAAAAAAAAAAAAAAAAAAAAAAAAIAAQATABwAAA==";

SerializableContainer getDefaultFont() {
    std::vector<unsigned char> dfcont{defaultFont64.begin(), defaultFont64.end()};
    auto cont = bn::decode_b64(dfcont);
    return cont;
}