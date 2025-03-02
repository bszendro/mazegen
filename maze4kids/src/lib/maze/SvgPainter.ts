import { type Painter, PaintStyle, type Point2D } from './Painter';

interface PainterParams {
    strokeWidth: number;
}

const SVG_NAMESPACE = 'http://www.w3.org/2000/svg';

export class SvgPainter implements Painter {
    private node: Node;
    private params: PainterParams;
    private svg: SVGSVGElement | undefined;

    constructor(node: Node, params: PainterParams) {
        this.node = node;
        this.params = params;
    }

    BeginDraw(width: number, height: number): void {
        const svg = document.createElementNS(SVG_NAMESPACE, 'svg');
        svg.setAttribute('width', width.toString());
        svg.setAttribute('height', height.toString());
        svg.style.backgroundColor = 'white';
        this.svg = svg;
    }

    EndDraw(): void {
        this.node.appendChild(this.svg!);
    }

    DrawLine(p1: Point2D, p2: Point2D, style: PaintStyle): void {
        const line = document.createElementNS(SVG_NAMESPACE, 'line');
        line.setAttribute('x1', p1.x.toString());
        line.setAttribute('y1', p1.y.toString());
        line.setAttribute('x2', p2.x.toString());
        line.setAttribute('y2', p2.y.toString());
        this.setStyle(line.style, style);
        this.svg!.appendChild(line);
    }

    DrawPoly(vertices: Point2D[], style: PaintStyle): void {
        const polygon = document.createElementNS(SVG_NAMESPACE, 'polygon');
        polygon.setAttribute('points', vertices.map(v => `${v.x},${v.y}`).join(' '));
        this.setStyle(polygon.style, style);
        this.svg!.appendChild(polygon);
    }

    private setStyle(cssStyle: CSSStyleDeclaration, style: PaintStyle) {
      switch (style) {
        case PaintStyle.OpenCell:
          cssStyle.fill = 'white';
          break;

        case PaintStyle.VisitedCell:
          cssStyle.fill = 'white';
          break;

        case PaintStyle.OnPathCell:
          cssStyle.fill = 'lightgray';
          break;

        case PaintStyle.Wall:
          cssStyle.stroke = 'black';
          cssStyle.strokeWidth = this.params.strokeWidth.toString();
          cssStyle.strokeLinecap = 'round';
          break;

        case PaintStyle.WallBlocked:
          cssStyle.stroke = 'black';
          cssStyle.strokeWidth = this.params.strokeWidth.toString();
          cssStyle.strokeLinecap = 'round';
          break;
      }
    }
}
