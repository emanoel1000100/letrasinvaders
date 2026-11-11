import pathlib
import re
import unittest
ROOT = pathlib.Path(__file__).resolve().parents[1]
class InterfaceTests(unittest.TestCase):
    def test_indicator_is_not_abbreviated(self):
        text = (ROOT / 'src/keyboard.c').read_text()
        self.assertIn('"INDICADOR ESQUERDO"', text)
        self.assertNotIn('"INDICADOR E"', text)
    def test_instruction_removed(self):
        self.assertNotIn('DIGITE A LETRA CERTA SEM OLHAR', (ROOT/'src/render.c').read_text())
    def test_arena_is_40_percent_taller(self):
        text = (ROOT/'include/game.h').read_text()
        height = int(re.search(r'#define SCREEN_H (\d+)', text)[1])
        self.assertAlmostEqual((height-228-110)/(720-228-110),1.4,delta=0.01)
if __name__ == '__main__':
    unittest.main()
