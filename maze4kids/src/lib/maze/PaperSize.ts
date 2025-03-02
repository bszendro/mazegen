
export interface Size2D {
  width: number;
  height: number;
}

export type PaperSize = 'a3' | 'a4' | 'a5';

const MARGIN_X = 0.39; // ~1cm
const MARGIN_Y = 0.39; // ~1cm
const PPI = 96;

export const getPaperSizeInInches = (paperSize: PaperSize): Size2D => {
  switch (paperSize) {
    case 'a3': return { width: 11.7, height: 16.5 };
    case 'a4': return { width: 8.3, height: 11.7 };
    case 'a5': return { width: 5.8, height: 8.3 };
  }
}

/** Compute the rectangle available (in pixels) for drawing the maze based on the paper size */
export const getAreaSize = (paperSize: PaperSize): Size2D => {
  const size = getPaperSizeInInches(paperSize);
  return inchSizeToPixelSize({
    width: size.width - 2*MARGIN_X,
    height: size.height - 2*MARGIN_Y,
  });
}

export const inchSizeToPixelSize = (inchSize: Size2D): Size2D => ({
  width: Math.floor(inchSize.width * PPI),
  height: Math.floor(inchSize.height * PPI),
});
